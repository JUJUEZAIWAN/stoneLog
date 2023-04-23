add_rules("mode.debug", "mode.release")

target("stonelog")
    set_kind("binary")
    add_files("src/*.cpp")
    add_includedirs("include")
    
target("AsyncLog")
    set_kind("binary")
    add_files("test/testAsyncLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")
    
target("SyncLog")
    set_kind("binary")
    add_files("test/testSyncLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")

target("SyncTcp")
    set_kind("binary")
    add_files("test/testSyncTcpLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")

target("SyncUdp")
    set_kind("binary")
    add_files("test/testSyncUdpLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")

target("AsyncTcp")
    set_kind("binary")
    add_files("test/testAsyncTcpLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")

target("AsyncUdp")
    set_kind("binary")
    add_files("test/testAsyncUdpLog.cpp")
    add_includedirs("include")
    add_files("src/*.cpp")
    

