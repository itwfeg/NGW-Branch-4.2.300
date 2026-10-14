import installUtils

company = 'itw'
product = 'test_station'
feature_name = 'cpu_memory_tester'
version = '1.0.0'
file_patterns_to_add = ["*.py",
                        "*.sh",
                        "memtester",
                        "*.desktop",
                        "cpu_memory_tester.png"]

installUtils.build_runtime_tarball(feature_name=feature_name,
                                   file_patterns_to_add=file_patterns_to_add)

installUtils.build_installer_tarball(feature_name=feature_name,
                                     version=version,
                                     company=company,
                                     product=product)
