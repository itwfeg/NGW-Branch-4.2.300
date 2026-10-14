

import sqlite3
import logging

from collections import OrderedDict as OrderedDictionary


def escape_quotes(value):
    if not value:
        return '""'

    elif value.count('"') <= 2:
        return value

    return value.replace("\"", "\"\"\"")


def create_connection(database_path):
    database_connection = sqlite3.connect(database_path)
    # database_connection.text_factory = lambda x: unicode(x, "windows-1252", "replace")
    database_connection.row_factory = sqlite3.Row

    return database_connection


class DatabaseError(Exception):
    pass


class IgnoredColumnException(Exception):
    pass


class Database(object):
    def __init__(self, database_path, table=None):
        self.database_path = database_path
        self.database_connection = create_connection(self.database_path)
        self.table = table
        self._execute_debug_template = 'Executing database command {command}'

        logging.info('Database file path: {0}'.format(self.database_path))
        logging.info('Database table: {0}'.format(self.table))

    @staticmethod
    def assignment_clause(assignments, separator=', '):
        formatted_assignments = []

        for key, value in assignments.items():
            # Don't quote integers.
            if isinstance(value, int):
                formatted_assignments.append('{0}={1}'.format(key, escape_quotes(str(value))))
            else:
                formatted_assignments.append('{0}="{1}"'.format(key, escape_quotes(str(value))))

        return separator.join(formatted_assignments)

    def where_clause(self, where):
        if not where:
            return ''

        return ' WHERE ' + self.assignment_clause(where, separator=' AND ')

    @staticmethod
    def value_clause(values):
        return "'{0}'".format("', '".join(escape_quotes(value) for value in values))

    @staticmethod
    def column_clause(columns):
        if not columns:
            return '*'

        return '{0}'.format(', '.join(escape_quotes(column) for column in columns))

    def execute(self, command):
        database_cursor = self.database_connection.cursor()

        try:
            # logging.info(self._execute_debug_template.format(command=command))
            database_cursor.execute(command)
        except sqlite3.Error as err:
            self.database_connection.rollback()
            raise DatabaseError(str(err))
        else:
            results = database_cursor.fetchall()
            self.database_connection.commit()
            return results

    def select(self, columns=None, where=None):
        """ Execute a query on the database using any given criteria on the current table.

        :param columns: list[str] specifying what columns to select.
        :param where: dict[str, str] specifying constraints to select by.
        :return: list[SqliteRow]
        """
        select_template = 'SELECT {columns} FROM {table}{where}'
        column_clause = self.column_clause(columns)
        where_clause = self.where_clause(where)
        return self.execute(select_template.format(columns=column_clause, table=self.table, where=where_clause))

    def select_all(self):
        return self.select()

    def insert(self, data, replace=False):
        insert_template = 'INSERT{replace} INTO {table} ({columns}) VALUES ({values})'
        column_clause = self.column_clause(data.keys())
        value_clause = self.value_clause(data.values())
        replace_clause = 'OR REPLACE' if replace else ''
        return self.execute(insert_template.format(replace=replace_clause,
                                                   table=self.table,
                                                   columns=column_clause,
                                                   values=value_clause))

    def update(self, values, where=None):
        update_template = 'UPDATE {table} SET {values}{where}'
        value_clause = self.assignment_clause(values)
        where_clause = self.where_clause(where)
        return self.execute(update_template.format(table=self.table, values=value_clause, where=where_clause))

    def delete(self, where=None):
        delete_template = 'DELETE FROM {table}{where}'
        where_clause = self.where_clause(where)
        return self.execute(delete_template.format(table=self.table, where=where_clause))

    def header(self):
        pragma_template = 'PRAGMA table_info({table})'
        header = Header()
        results = self.execute(pragma_template.format(table=self.table))

        for data_row in results:
            header.add_column(data_row)

        return header

    def create_table(self, name, columns, primary_key, ignore_exists=False, with_rowid=True):
        create_template = 'CREATE TABLE{exists} {name} ({columns}, {primary_key}){rowid}'
        formatted_columns = []
        primary_key_clause = 'primary key ({0})'.format(primary_key)
        exists_clause = ' IF NOT EXISTS ' if ignore_exists else ''
        rowid_clause = ' WITHOUT ROWID' if not with_rowid else ''

        for column_name, column_type in columns.items():
            formatted_columns.append('{0} {1}'.format(column_name, column_type.upper()))

        return self.execute(create_template.format(exists=exists_clause,
                                                   name=name,
                                                   columns=', '.join(formatted_columns),
                                                   primary_key=primary_key_clause,
                                                   rowid=rowid_clause))


class Column(object):
    def __init__(self, data_row):
        self.id, self.name, self.type, self.not_null, self.default_value, self.primary_key = data_row


class Header(object):
    def __init__(self):
        self._columns = OrderedDictionary()
        self._primary_keys = OrderedDictionary()

    @property
    def columns(self):
        return self._columns

    @property
    def primary_keys(self):
        return self._primary_keys

    def add_column(self, data_row):
        column_object = Column(data_row)
        self._columns[column_object.name] = column_object

        if column_object.primary_key:
            self._primary_keys[column_object.name] = column_object
