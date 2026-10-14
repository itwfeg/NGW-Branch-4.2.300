

def read_file_into_buff(filename, mode='r'):
    try:
        in_file = open(filename, mode)
        data = in_file.read()
        in_file.close()
    except IOError as e:
        return 'error: Can not open file: ' + filename + ', Reason:' + e, None

    return 'success', data


def read_from_file(filename, remove_cr=1):
    try:
        in_file = open(filename, 'r')
    except IOError as e:
        return 'error: Can not open file: ' + filename + ', Reason:' + e, None

    data = in_file.readlines()

    if remove_cr:
        for i in range(0, len(data)):
            if data[i][-1:] == '\n':
                data[i] = data[i][:-1]

    in_file.close()
    return 'success', data


def write_to_file(filename, data):
    try:
        output_file = open(filename, 'w')
    except IOError as e:
        return 'error: Can not open file: ' + filename + ', Reason:' + e, None

    for i in range(0, len(data)):
        output_file.write(data[i] + '\n')
    output_file.close()

    return 'success', None


def write_buffer_to_file(filename, data, mode='w'):
    try:
        output_file = open(filename, mode)
    except IOError as e:
        return 'error: Can not open file: ' + filename + ', Reason:' + e, None

    output_file.write(data)
    output_file.close()

    return 'success', None
