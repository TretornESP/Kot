#include <core/main.h>

#include <kot-ui++/pictures/picture.h>
using namespace Ui;

#include <kot++/printf.h>

// todo: fonction "AddWallpaper" qui permet d'ajouter un wallpaper dans "USER/Share/Wallpaper" et de modifier les parametres "desktop.json" pour changer le fit en fonction de la taille de l'image (modifiable par l'utilisateur)

// todo: fonction "ChangeFit"

desktopc::desktopc(JsonObject* DesktopSettings)
{
    window_t* Desktop = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Desktop, Window_Max_Size, Window_Max_Size);

    Fb = &Desktop->Framebuffer;

    UiCtx = new Ui::UiContext(Fb);

    /* desktop background */
    JsonObject* Background = (JsonObject*)DesktopSettings->Get("background");
    bool IsWallpaper = ((JsonBoolean*)Background->Get("isWallpaper"))->Get();
    char* WallpaperPath = ((JsonString*)Background->Get("wallpaperPath"))->Get();
    uint8_t WallpaperFit = ((JsonNumber*)Background->Get("wallpaperFit"))->Get();
    uint32_t SolidColor = strtol(((JsonString*)Background->Get("solidColor"))->Get(), NULL, 16);

    if(IsWallpaper)
        SetWallpaper(WallpaperPath, (PictureboxFit) WallpaperFit);
    else
        SetSolidColor(SolidColor);

    /* clock */
    bool IsClock = ((JsonBoolean*)Background->Get("isClock"))->Get();
    char* FontPathClock = ((JsonString*)Background->Get("clockFontPath"))->Get();

    if(IsClock)
        InitalizeClock(FontPathClock);

    /* applications */
    // todo: creer une grille pour les icones

    UiCtx->UiStartRenderer();
    ChangeVisibilityWindow(Desktop, true);
}

void desktopc::CreateTaskbar(JsonObject* TaskbarSettings) {
    Taskbar = (Taskbar_t*) malloc(sizeof(Taskbar_t));

    JsonObject* Style = (JsonObject*)TaskbarSettings->Get("style");

    Taskbar->Height = 65;
    Taskbar->IsExtended = ((JsonBoolean*) Style->Get("isExtended"))->Get();
    Taskbar->IconSize = ((JsonNumber*) Style->Get("iconSize"))->Get();

    Taskbar->Window = CreateWindow(NULL, Window_Type_DockBottom);
    ResizeWindow(Taskbar->Window, Window_Max_Size, Taskbar->Height);

    framebuffer_t* TaskbarFb = &Taskbar->Window->Framebuffer;

    UiContext* UiCtx = new UiContext(TaskbarFb);

    Box_t* AppsContainer = Box(
        {
            .G = {
                    .Width = 500,
                    .Height = Taskbar->Height
                },
            .BackgroundColor = 0xFFFF00
        }
    , UiCtx->Cpnt);
    
    JsonArray* Applications = (JsonArray*)TaskbarSettings->Get("applications");
    
    for(uint8_t i = 0; i < Applications->length(); i++) {
        JsonObject* App = (JsonObject*)Applications->Get(i);

        uint8_t AppPosition = ((JsonNumber*) App->Get("position"))->Get();

        Box_t* AppButton = Box(
        {
            .G = {
                    .Width = Taskbar->Height,
                    .Height = Taskbar->Height,
                    .BorderRadius = 15,
                    .Position = { .x = AppPosition * Taskbar->Height }
                },
            .BackgroundColor = 0xFF0000
        }
        , AppsContainer->Cpnt);

        Picturebox_t* AppIcon = Picturebox("kotlogo.tga", PictureboxType::_TGA,
            {
                .G = {
                        .Width = Taskbar->IconSize,
                        .Height = Taskbar->IconSize,
                        .Position = {
                            .x = Taskbar->Height - Taskbar->IconSize,
                            .y = Taskbar->Height - Taskbar->IconSize
                        }
                    }
            }
        , AppButton->Cpnt);
    }

    UiCtx->UiStartRenderer();
    ChangeVisibilityWindow(Taskbar->Window, true);
}

void desktopc::ResizeTaskbar() {
    // todo
}

void desktopc::SetWallpaper(char* Path, PictureboxFit Fit) {
    Wallpaper = Ui::Picturebox(Path, _TGA, 
        {
            .Fit = Fit,
            .G = {
                    .Width = -100,
                    .Maxwidth = NO_MAXIMUM,
                    .Height = -100,
                    .Maxheight = NO_MAXIMUM,
                    .Position = {
                        .x = 0,
                        .y = 0
                    },
                    .IsHidden = false
                }
        }
    , UiCtx->Cpnt);
}

void desktopc::SetSolidColor(uint32_t Color){
    FillRect(Fb, 0, 0, Fb->Width, Fb->Height, Color);
}

char* WeekdayString[] = {
    "sunday",
    "monday",
    "tuesday",
    "wednesday",
    "thurday",
    "friday",
    "saturday",
};

char* MonthString[] = {
    "january",
    "february",
    "march",
    "april",
    "may",
    "june",
    "july",
    "august",
    "september",
    "october",
    "november",
    "december",
};

void UpdateClock(Ui::Label_t* Time, Ui::Label_t* Date){
    char TimeStr[1024];
    char DateStr[1024];
    uint64_t TimerState;

    if(GetSecond()){
        Sleep((60 - GetSecond()) * 1000);
    }
    GetActualTick(&TimerState);

    while(true){
        sprintf((char*)&TimeStr, "%d:%d", GetHour(), GetMinute());
        Time->UpdateText((char*)&TimeStr);
        sprintf((char*)&DateStr, "%d %s, %d", GetDay(), MonthString[GetMonth()-1], 2023);
        Date->UpdateText((char*)&DateStr);
        SleepFromTick(&TimerState, 60000);
    }
    
    Sys_Close(KSUCCESS);
}

void desktopc::InitalizeClock(char* FontPath){
    Ui::Flexbox_t* ClockContainer = Ui::Flexbox(
        {
            .Direction = Layout::COLUMN,
            .G = { 
                    .Width = -100,
                    .Height = -100,
                    .Position{
                        .x = 0,
                        .y = 0
                    },
                    .AutoPosition = false, 
                }, 
            .Align = { 
                .x = Layout::FILLHORIZONTAL, 
                .y = Layout::TOP 
            }
        }
    , UiCtx->Cpnt);

    char TimeStr[1024];
    sprintf((char*)&TimeStr, "%d:%d", GetHour(), GetMinute());

    Ui::Label_t* Time = Ui::Label(
        {
            .Text = TimeStr,
            .FontPath = FontPath,
            .FontSize = 72,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNCENTER,
            .AutoWidth = false,
            .AutoHeight = true,
            .G = {
                    .Margin = {
                        .Top = 20,
                    },
                    .Width = -100,
                    .AutoPosition = false,        
                }
        }
    , ClockContainer->Cpnt);

    char DateStr[1024];
    sprintf((char*)&DateStr, "%d %s, %d", GetDay(), MonthString[GetMonth()-1], 2023);

    Ui::Label_t* Date = Ui::Label(
        {
            .Text = DateStr,
            .FontPath = FontPath,
            .FontSize = 16,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNCENTER,
            .AutoWidth = false,
            .AutoHeight = true,
            .G = {
                    .Margin = {
                        .Top = 20,
                    },
                    .Width = -100, 
                    .AutoPosition = false,          
                }
        }
    , ClockContainer->Cpnt);

    Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&UpdateClock, PriviledgeApp, NULL, &ClockThread);
    
    arguments_t Parameters{
        .arg[0] = (uint64_t)Time,
        .arg[1] = (uint64_t)Date,
    };
    Sys_ExecThread(ClockThread, &Parameters, ExecutionTypeQueu, NULL);
}

extern "C" int main(int argc, char *argv[])
{
    Printlog("[DESKTOP] Initializing...");

    // load desktopUserSettings.json

    file_t* SettingsFile = fopen("d1:Usr/root/Share/Settings/desktop.json", "r");

    if (SettingsFile == NULL)
    {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m File not found.");
        return KFAIL;
    }

    fseek(SettingsFile, 0, SEEK_END);
    size_t SettingsFileSize = ftell(SettingsFile);
    fseek(SettingsFile, 0, SEEK_SET);

    char* BufferSettingsFile = (char*)malloc(SettingsFileSize);
    fread(BufferSettingsFile, SettingsFileSize, 1, SettingsFile);

    JsonParser* parser = new JsonParser(BufferSettingsFile);

    if (parser->getCode() != JSON_SUCCESS && parser->getValue()->getType() != JSON_ARRAY)
    {
        Printlog("[DESKTOP] \033[0;31mERR:\033[0m Json file could not be retrieved.");
        return KFAIL;
    }

    JsonArray* Settings = (JsonArray*)parser->getValue();

    JsonObject* DesktopSettings = (JsonObject*)Settings->Get(0);
    desktopc* desktop0 = new desktopc(DesktopSettings);

    JsonObject* TaskbarSettings = (JsonObject*)Settings->Get(1);
    //desktop0->CreateTaskbar(TaskbarSettings);

    fclose(SettingsFile);

    Printlog("[DESKTOP] Initialized");
    return KSUCCESS;
}