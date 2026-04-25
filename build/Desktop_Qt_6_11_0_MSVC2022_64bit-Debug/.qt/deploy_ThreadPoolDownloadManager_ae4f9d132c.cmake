include("E:/software/qt/qtproject/ThreadPoolDownloadManager/build/Desktop_Qt_6_11_0_MSVC2022_64bit-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/ThreadPoolDownloadManager-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "E:/software/qt/qtproject/ThreadPoolDownloadManager/build/Desktop_Qt_6_11_0_MSVC2022_64bit-Debug/ThreadPoolDownloadManager.exe"
    GENERATE_QT_CONF
)
