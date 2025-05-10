#include <windows.h>
#include <string>
#include <filesystem>

void SetSkyrimWindow(HWND hwnd, HICON hNewIcon) {
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hNewIcon);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hNewIcon);

    SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hNewIcon);
    SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hNewIcon);
}

void SetSkyrimWindow(HWND hwnd, HICON hNewIcon, LPCSTR hNewTitle) {
    SetSkyrimWindow(hwnd, hNewIcon);
    SetWindowTextA(hwnd, hNewTitle);
}

std::string mainProcess() {    
    std::string pluginspath = std::filesystem::current_path().string();
    pluginspath.append("\\data\\SKSE\\Plugins\\");

    std::string inipath = pluginspath;
    inipath.append("SKSEIconAndTitleSwapper.ini");

    const int imgwidth = GetPrivateProfileIntA("Settings", "imgwidth", 0, inipath.c_str());
    const int imgheight = GetPrivateProfileIntA("Settings", "imgheight", 0, inipath.c_str());

    char skyrimtitle[256] = { 0 };
    GetPrivateProfileStringA("Settings", "skyrimtitle", "Skyrim Special Edition", skyrimtitle, sizeof(skyrimtitle), inipath.c_str());
    std::wstring windowTitle(skyrimtitle, skyrimtitle + strlen(skyrimtitle));

    char customtitle[256] = { 0 };
    GetPrivateProfileStringA("Settings", "customtitle", "none", customtitle, sizeof(customtitle), inipath.c_str());
    
    
    HWND hwnd = FindWindow(NULL, windowTitle.c_str());
    if (!hwnd) return "Failed to find game window";

    std::string iconpath = pluginspath;
    iconpath.append("SKSEIconAndTitleSwapper/icon.ico");
    std::wstring wiconpath = std::filesystem::path(iconpath).wstring();

    HICON icon = (HICON)LoadImage(NULL, wiconpath.c_str(), IMAGE_ICON, imgwidth, imgheight, LR_LOADFROMFILE);

    if (!icon) return "Failed to load custom icon";
    
    if (strcmp(customtitle, "none") == 0) SetSkyrimWindow(hwnd, icon);
    else SetSkyrimWindow(hwnd, icon, (LPCSTR)customtitle);

    return "Window or taskbar icon and maybe even the app title is theoretically changed! :D \n(Icon details: " + std::to_string(imgwidth) + "x" + std::to_string(imgheight) + ", Custom title: " + customtitle + ")";
}


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);
       

    // Once all plugins and mods are loaded, then the ~ console is ready and can
    // be printed to    
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {                                    
            //Yes it is fine to call the process at this point, calling it eariler might result in not being able to find the hwnd
            std::string result = mainProcess();            
            RE::ConsoleLog::GetSingleton()->Print(result.c_str());              
        }
    });    


    return true;
}