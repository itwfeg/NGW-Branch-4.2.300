from PyQt5.QtCore import *
from PyQt5.QtCore import pyqtSignal

import cpu_memory_tests


class Worker(QThread):

    alert_handler = pyqtSignal(str)
    test_complete_handler = pyqtSignal(str)
    test_status_update_handler = pyqtSignal(dict, str)
    elapsed_time_handler = pyqtSignal(str)

    def __init__(self,
                 host_range,
                 minutes_to_run_test,
                 parent=None):

        QThread.__init__(self, parent)

        self.overall_test_status = 'FAIL'
        self.host_range = host_range
        self.minutes_to_run_test = minutes_to_run_test
        self.cpu_memory_tester = \
            cpu_memory_tests.CpuMemoryTestFacility(
                host_range=self.host_range,
                minutes_to_run=self.minutes_to_run_test,
                alert_handler_callback=self.alert_handler_callback,
                progress_handler_callback=None,
                device_scan_handler_callback=None,
                elapsed_time_handler_callback=self.elapsed_time_handler_callback,
                test_status_update_handler_callback=
                self.test_status_update_handler_callback)

    def get_host_details(self):
        return self.cpu_memory_tester.get_host_details()

    def alert_handler_callback(self, message):
        self.alert_handler.emit(message)

    def test_status_update_handler_callback(self, hosts_status, overall_test_status):
        self.test_status_update_handler.emit(hosts_status, overall_test_status)
        self.overall_test_status = overall_test_status

    def elapsed_time_handler_callback(self, elapsed_time):
        self.elapsed_time_handler.emit(elapsed_time)

    def stop_test(self):
        self.cpu_memory_tester.abort_running_tests()

    def __del__(self):
        self.wait()

    def run(self):

        self.cpu_memory_tester.start_memtesters()
        self.cpu_memory_tester.check_status_of_memtesters()
        self.cpu_memory_tester.stop_memtesters()
        # self.cpu_memory_tester.get_memtester_logs()
        self.cpu_memory_tester.cleanup()
        self.test_complete_handler.emit(self.overall_test_status)
