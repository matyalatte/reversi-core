project('reversi-core', 'c',
    default_options: [
        'c_std=c99',
        'cpp_std=c++11',
    ],
    meson_version: '>=0.48.0',
    version: '0.1.0')

reversi = library('reversi',
    'src/reversi.c',
    install: true,
    include_directories: include_directories('./include'),
	gnu_symbol_visibility: 'hidden')
install_headers('include/reversi.h')

reversi_dep = declare_dependency(include_directories: include_directories('./include'),
	link_with : reversi)

if get_option('examples')
    executable('example',
        'src/example.c',
        dependencies: reversi_dep,
        install : true)
endif

if get_option('tests')
    add_languages('cpp', required: true)

    # get gtest
    gtest_proj = subproject('gtest')
    gtest_dep = gtest_proj.get_variable('gtest_dep')
    gmock_dep = gtest_proj.get_variable('gmock_dep')

    # build tests
    subdir('tests')
endif
