compile_ipc(DiagnosticServer.ipc DiagnosticServerEndpoint.h)
compile_ipc(DiagnosticClient.ipc DiagnosticClientEndpoint.h)

set(SOURCES
    ClientConnection.cpp
    DiagnosticDialog.cpp
    main.cpp
    DiagnosticServerEndpoint.h
    DiagnosticClientEndpoint.h
)

serenity_bin(DiagnosticServer)
target_link_libraries(DiagnosticServer LibGUI LibIPC)