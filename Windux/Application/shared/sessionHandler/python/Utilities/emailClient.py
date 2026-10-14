#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from exchangelib import DELEGATE, IMPERSONATION, Account, Credentials, EWSDateTime, EWSTimeZone, Configuration, NTLM, CalendarItem, Message, Mailbox, Attendee
from exchangelib.folders import Calendar, ExtendedProperty, FileAttachment, ItemAttachment, HTMLBody
import tzlocal

import smtplib
from email.mime.text import MIMEText

class EmailClient(object):
    def __init__(self,  server,  username,  password,  emailAddress):
        self.server = server
        self.username = username
        self.password = password
        self.emailAddress = emailAddress
        self.mailServerType = 'exchange'
        self.localTz = self.__getLocalTz()

    def __getLocalTz(self):
        return tzlocal.get_localzone()

class ExchangeServer(EmailClient):
    def __init__(self, server, username, password, emailAddress):
        EmailClient.__init__(self, server, username, password, emailAddress)
        self.credentials = Credentials(username=self.username, password=self.password, is_service_account=False)
        self.configuration = Configuration(server=self.server, credentials=self.credentials)
        self.account = Account(primary_smtp_address=self.emailAddress,
                                                access_type=DELEGATE, autodiscover=True, credentials=self.credentials)

    def sendEmail(self,to,subject,body):

        m = Message(
            account=self.account,
            subject=subject,
            body=body,
            to_recipients=[Mailbox(email_address=to)]
        )
        m.send()

class SmtpServer(EmailClient):
    def __init__(self, server, username, password, emailAddress):
        EmailClient.__init__(self, server, username, password, emailAddress)
        self.mailServerType = 'smtp'

    def sendEmail(self, to, subject, body):
        msg = MIMEText(body)
        msg['Subject'] = subject
        msg['From'] = self.emailAddress
        msg['To'] = to
        s = smtplib.SMTP(self.server)
        s.sendmail(self.emailAddress, [to], msg.as_string())
        s.quit()

if __name__ == '__main__':
    exchange = ExchangeServer("hybrid.itwfeg.com", "FEG\davisrs", "jen31Wife", "scott.davis@itwfeg.com")
    exchange.sendEmail("rsdavis@linuxden.com", "test message from exchangelib", "This is the body of the message that exchangelib sent.")
    smtp = SmtpServer("www.linuxden.com", "rsdavis", "password", "rsdavis@linuxden.com")
    smtp.sendEmail("rsdavis@linuxden.com", "test message from exchangelib", "This is the body of the message that exchangelib sent.")
