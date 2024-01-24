test_exe = executable('reversi_test',
    'main.cpp',
    dependencies : [reversi_dep, gtest_dep, gmock_dep],
    include_directories: include_directories('.'),
    install : false)

test('reversi_test', test_exe)
