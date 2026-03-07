#include "pch.h"
#include "cMain.h"
#include "FixFilePerms.h"
#include "inject.h"
#include "config.h"
#include "icon/icon.xpm"

namespace
{
    constexpr int ID_BUTTON_INJECT = 101;
    constexpr int ID_BUTTON_HIDE = 102;
    constexpr int ID_BUTTON_SELECT = 103;
    constexpr int ID_CHECKBOX_CUSTOM = 201;
    constexpr int ID_CHECKBOX_AUTO = 202;
    constexpr int ID_TIMER_AUTO_INJECT = 401;
}

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_BUTTON(ID_BUTTON_INJECT, OnInjectButton)
EVT_BUTTON(ID_BUTTON_HIDE, OnHideButton)
EVT_BUTTON(ID_BUTTON_SELECT, OnSelectButton)
EVT_CHECKBOX(ID_CHECKBOX_CUSTOM, OnCustomCheckBox)
EVT_CHECKBOX(ID_CHECKBOX_AUTO, OnAutoCheckBox)
EVT_TIMER(ID_TIMER_AUTO_INJECT, OnAutoInjectTimer)
wxEND_EVENT_TABLE();

cMain::cMain()
    : wxFrame(nullptr, wxID_ANY, "Fate Client Injector", wxDefaultPosition, wxSize(297.5, 162.5), wxMINIMIZE_BOX | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
      autoInjectTimer(this, ID_TIMER_AUTO_INJECT)
{
    wxIcon icon(icon_xpm);
    SetIcon(icon);
    SetBackgroundColour(wxColour(255, 255, 255, 255));

    mainPanel = new wxPanel(this, wxID_ANY);
    btn_Inject = new wxButton(mainPanel, ID_BUTTON_INJECT, "Inject", wxPoint(5, 5), wxSize(100, 40));
    btn_Hide = new wxButton(mainPanel, ID_BUTTON_HIDE, "Hide Menu", wxPoint(5, 50), wxSize(100, 20));
    btn_Select = new wxButton(mainPanel, ID_BUTTON_SELECT, "Select", wxPoint(5, 75), wxSize(60, 20));
    txt_Name = new wxTextCtrl(mainPanel, wxID_ANY, "minecraft.windows.exe", wxPoint(110, 5), wxSize(165, 20));
    check_Custom = new wxCheckBox(mainPanel, ID_CHECKBOX_CUSTOM, "Custom Target", wxPoint(110, 30), wxSize(165, 20));
    check_Auto = new wxCheckBox(mainPanel, ID_CHECKBOX_AUTO, "Auto Inject", wxPoint(110, 50), wxSize(130, 20));
    txt_Delay = new wxTextCtrl(mainPanel, wxID_ANY, "5", wxPoint(245, 50), wxSize(30, 20), wxTE_CENTRE, wxTextValidator(wxFILTER_NUMERIC));
    txt_Delay->SetMaxLength(4);
    txt_Path = new wxTextCtrl(mainPanel, wxID_ANY, "Click \"Select\" to select the dll file", wxPoint(70, 75), wxSize(205, 20));

    CreateStatusBar(1);
    SetStatusText("Version 1.0 | Made by youtube.com/fligger", 0);

    check_Custom->SetValue(customProcName);
    txt_Delay->SetValue(delaystr);
    txt_Path->SetValue(dllPath);
    txt_Name->SetValue(procName);

    if (!customProcName)
    {
        txt_Name->Disable();
    }

    taskBarControl.SetIcon(icon, "Double-Click to show Fate Injector");

    notification = new wxNotificationMessage("Fate Client Injector", "Fate Client Injector is now hidden in your system tray", this, 0);
    notification->UseTaskBarIcon(&taskBarControl);
    openDialog = new wxFileDialog(this, "Select the .dll file", working_dir, "Fate.Client.dll", "Dynamic link library (*.dll)|*.dll", wxFD_OPEN);
}

cMain::~cMain()
{
    autoInjectTimer.Stop();
    openDialog->Destroy();
    notification->Close();
    delete notification;
}

void cMain::OnInjectButton(wxCommandEvent& evt)
{
    OnInjectButtonExecute(evt, this);
}

void cMain::OnInjectButtonExecute(wxCommandEvent& evt, cMain* ref)
{
    if (ref != nullptr)
    {
        ref->InjectCurrentTarget(false);
    }

    evt.Skip();
}

void cMain::OnHideButton(wxCommandEvent& evt)
{
    notification->Show();
    Hide();
    evt.Skip();
}

void cMain::OnSelectButton(wxCommandEvent& evt)
{
    if (openDialog->ShowModal() == wxID_OK)
    {
        txt_Path->SetValue(openDialog->GetPath());
    }

    SaveCurrentConfig();
    evt.Skip();
}

void cMain::OnCustomCheckBox(wxCommandEvent& evt)
{
    if (check_Custom->IsChecked())
    {
        txt_Name->Enable(true);
    }
    else
    {
        txt_Name->Enable(false);
        txt_Name->SetValue("minecraft.windows.exe");
    }

    SaveCurrentConfig();
    evt.Skip();
}

void cMain::OnAutoCheckBox(wxCommandEvent& evt)
{
    if (check_Auto->IsChecked())
    {
        txt_Name->Disable();
        txt_Path->Disable();
        txt_Delay->Disable();
        btn_Select->Disable();
        check_Custom->Disable();

        lastAutoInjectedProcId = 0;
        int delay = GetValidatedDelaySeconds();
        autoInjectTimer.Start(delay * 1000);
        SetStatusText("AutoInject: Enabled | trying every " + std::to_string(delay) + " seconds", 0);
    }
    else
    {
        autoInjectTimer.Stop();
        disableAutoInject();
        SetStatusText("AutoInject: Disabled", 0);
    }

    SaveCurrentConfig();
    evt.Skip();
}

void cMain::OnAutoInjectTimer(wxTimerEvent& evt)
{
    if (!check_Auto->IsChecked())
    {
        autoInjectTimer.Stop();
        disableAutoInject();
        evt.Skip();
        return;
    }

    DWORD procId = GetProcId(txt_Name->GetValue().mb_str());
    if (procId == 0)
    {
        SetStatusText("AutoInject: Can't find process! | 0", 0);
        evt.Skip();
        return;
    }

    if (procId == lastAutoInjectedProcId)
    {
        SetStatusText("AutoInject: Already injected! | " + std::to_string(procId), 0);
        evt.Skip();
        return;
    }

    DWORD injectedProcId = 0;
    if (InjectCurrentTarget(true, &injectedProcId))
    {
        lastAutoInjectedProcId = injectedProcId;
    }
    else
    {
        if (GetProcId(txt_Name->GetValue().mb_str()) != 0)
        {
            check_Auto->SetValue(false);
            autoInjectTimer.Stop();
            disableAutoInject();
        }
    }

    evt.Skip();
}

void cMain::disableAutoInject()
{
    check_Custom->Enable();
    if (check_Custom->IsChecked())
    {
        txt_Name->Enable();
    }
    else
    {
        txt_Name->Disable();
    }

    txt_Path->Enable();
    txt_Delay->Enable();
    btn_Select->Enable();
}

int cMain::GetValidatedDelaySeconds()
{
    long delay = 0;
    if (!txt_Delay->GetValue().ToLong(&delay) || delay < 1)
    {
        delay = 1;
    }
    if (delay > 3600)
    {
        delay = 3600;
    }

    txt_Delay->SetValue(wxString::Format("%ld", delay));
    return static_cast<int>(delay);
}

void cMain::SaveCurrentConfig()
{
    customProcName = check_Custom->GetValue();
    delaystr = txt_Delay->GetValue();
    dllPath = txt_Path->GetValue();
    procName = txt_Name->GetValue();

    config cfg;
    cfg.saveConfig();
}

bool cMain::InjectCurrentTarget(bool autoInjectMode, DWORD* injectedProcId)
{
    std::string prefix;
    if (autoInjectMode)
    {
        prefix = "AutoInject: ";
    }

    DWORD procId = GetProcId(txt_Name->GetValue().mb_str());
    if (procId == 0)
    {
        SetStatusText(prefix + "Can't find process! | 0", 0);
        return false;
    }

    std::wstring dllPathValue = txt_Path->GetValue().ToStdWstring();
    PreflightResult preflight = RunInjectionPreflight(procId, dllPathValue.c_str());
    if (!preflight.ok)
    {
        std::string status = prefix + preflight.message;
        if (preflight.win32Error != 0)
        {
            status += " (error " + std::to_string(preflight.win32Error) + ": " + FormatWindowsError(preflight.win32Error) + ")";
        }
        SetStatusText(status, 0);
        return false;
    }

    DWORD accessError = 0;
    if (!SetAccessControl(dllPathValue, L"S-1-15-2-1", &accessError))
    {
        std::string status = prefix + "Failed to set DLL read/execute permissions.";
        if (accessError != 0)
        {
            status += " (error " + std::to_string(accessError) + ": " + FormatWindowsError(accessError) + ")";
        }
        SetStatusText(status, 0);
        return false;
    }

    InjectionResult result = performInjection(procId, dllPathValue.c_str());
    if (!result.ok)
    {
        std::string status = prefix + result.message;
        if (result.win32Error != 0)
        {
            status += " (error " + std::to_string(result.win32Error) + ": " + FormatWindowsError(result.win32Error) + ")";
        }
        SetStatusText(status, 0);
        return false;
    }

    SetStatusText(prefix + "Injected successfully into process " + std::to_string(procId), 0);
    if (injectedProcId != nullptr)
    {
        *injectedProcId = procId;
    }

    SaveCurrentConfig();
    return true;
}
