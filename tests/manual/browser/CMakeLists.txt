# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

find_package(${Qt} COMPONENTS Widgets Network REQUIRED)

add_executable(qlitehtmlbrowser
  MACOSX_BUNDLE
  main.cpp)

target_link_libraries(qlitehtmlbrowser qlitehtml ${Qt}::Widgets ${Qt}::Network)
