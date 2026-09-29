#include "editor/ForgeEditor.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

using forge::platform::InputEvent;
using forge::platform::Key;
using forge::platform::MouseButton;

namespace forge::editor {

static uint32_t C(uint32_t x) { return x; }

ForgeEditor::ForgeEditor(platform::NativeWindow& w, core::ForgeSystem& s)
    : window_(w), system_(s), projectLocation_(project::ProjectManager::defaultProjectsDirectory().string()) {
    window_.setEventHandler([this](const InputEvent& e) { onEvent(e); });
    window_.setPaintHandler([this](int w, int h) { paint(w, h); });
    std::string accountError; account_.load(accountError);
    authEmail_ = account_.profile().email;
    authDisplay_ = account_.profile().displayName;
    screen_ = account_.hasStoredAccount() ? ForgeScreen::Login : ForgeScreen::Signup;
    network_.configure(60, 8, 2);
    tests_.add("Engine System Registry", [this] { return system_.isReady("Renderer") && system_.isReady("Asset Database"); });
}

void ForgeEditor::run() { window_.invalidate(); window_.run(); }
void ForgeEditor::setStatus(const std::string& s) { status_ = s; log_ = s; window_.invalidate(); }
void ForgeEditor::drawPanel(int x, int y, int w, int h, uint32_t fill, uint32_t border) {
    if (w <= 0 || h <= 0) return;
    window_.drawRect(x, y, w, h, fill, true);
    window_.drawRect(x, y, w, h, border, false);
}

void ForgeEditor::drawButton(int x, int y, int w, int h, const std::string& text, bool active) {
    const bool hover = mouseX_ >= x && mouseX_ <= x + w && mouseY_ >= y && mouseY_ <= y + h;
    const uint32_t fill = active ? (hover ? C(0x2E78A8) : C(0x245D84)) : (hover ? C(0x252F3C) : C(0x1B222B));
    const uint32_t border = hover ? C(0x5A7C98) : C(0x354250);
    drawPanel(x, y, w, h, fill, border);
    window_.drawText(x + 10, y + std::max(16, h / 2 + 5), text, C(0xEAF0F6), 13);
}

void ForgeEditor::paint(int w, int h) {
    (void)w; (void)h;
    if (screen_ == ForgeScreen::Login) paintAuth(false);
    else if (screen_ == ForgeScreen::Signup) paintAuth(true);
    else if (screen_ == ForgeScreen::Hub) paintHub();
    else if (screen_ == ForgeScreen::CreateProject) paintCreate();
    else if (screen_ == ForgeScreen::Publish) paintPublish();
    else paintEditor();
}

void ForgeEditor::paintAuth(bool signup) {
    const int W=window_.width(), H=window_.height();
    window_.drawRect(0,0,W,H,C(0x080C11),true);
    window_.drawRect(0,0,W,72,C(0x101821),true);
    window_.drawText(34,45,"FORGE ENGINE",C(0xF4F7FA),24);
    window_.drawText(W-250,43,"PROFESSIONAL EDITION",C(0x687A8D),11);
    const int cw=std::min(560,W-80), x=(W-cw)/2;
    drawPanel(x,110,cw,signup?520:390,C(0x111922),C(0x2E3B48));
    window_.drawText(x+34,150,signup?"Create your Forge Account":"Welcome back",C(0xEAF0F5),24);
    window_.drawText(x+34,176,signup?"One account for projects, AI and publishing.":"Sign in to continue to Forge Hub.",C(0x7F8F9F),12);
    auto field=[&](int idx,int y,const std::string& label,const std::string& value,bool password=false){
        window_.drawText(x+34,y,label,C(0xB3C0CC),12);
        drawPanel(x+34,y+12,cw-68,40,authField_==idx?C(0x1C2A36):C(0x171F28),authField_==idx?C(0x51718A):C(0x30404C));
        std::string shown=value; if(password && !shown.empty()) shown=std::string(shown.size(),'*');
        if(authField_==idx) shown+="|";
        window_.drawText(x+48,y+38,shown,C(0xEFF4F7),13);
    };
    if(signup){
        field(0,210,"Display Name",authDisplay_); field(1,270,"Email",authEmail_); field(2,330,"Password",authPassword_,true); field(3,390,"Date of Birth (DD/MM/YYYY)",authDob_); field(4,450,"Country / Region",authCountry_);
        drawButton(x+34,510,190,38,"Create Account",true); drawButton(x+238,510,120,38,"Log In");
    } else {
        field(0,220,"Email",authEmail_); field(1,280,"Password",authPassword_,true);
        drawButton(x+34,350,190,38,"Log In",true); drawButton(x+238,350,160,38,"Create Account");
    }
    window_.drawText(x+34,H-34,status_,C(0x687A8C),11);
}

void ForgeEditor::paintHub() {
    const int W = window_.width(), H = window_.height();
    window_.drawRect(0, 0, W, H, C(0x0B0F14), true);
    window_.drawRect(0, 0, W, 64, C(0x111821), true);
    window_.drawText(30, 37, "FORGE ENGINE", C(0xF6F9FC), 24);
    window_.drawText(205, 35, "PROFESSIONAL", C(0x6F88A0), 13);
    window_.drawText(30, 95, "Forge Hub", C(0xDCE5EE), 22);
    window_.drawText(W-360, 96, "Account: " + (account_.profile().displayName.empty()?account_.profile().email:account_.profile().displayName), C(0x8092A4), 11);
    window_.drawText(30, 122, "Create, open and manage local game projects.", C(0x8190A1), 14);

    drawPanel(30, 150, std::min(760, W - 60), 140, C(0x121A22), C(0x2C3946));
    window_.drawText(52, 185, "START A NEW PROJECT", C(0xDDE7F0), 16);
    window_.drawText(52, 214, "Professional 2D/3D workspace with a real local project format.", C(0x7F8E9F), 13);
    drawButton(52, 238, 175, 38, "+  Create Project", true);
    drawButton(239, 238, 145, 38, "Open Project");

    window_.drawText(30, 335, "Recent Projects", C(0xDCE5EE), 17);
    auto ps = project::ProjectManager().discoverProjects(project::ProjectManager::defaultProjectsDirectory());
    if (ps.empty()) {
        drawPanel(30, 360, std::min(980, W - 60), 76, C(0x10171F), C(0x283440));
        window_.drawText(52, 407, "No projects found in the default ForgeProjects folder.", C(0x758598), 13);
    } else {
        int y = 360;
        for (const auto& p : ps) {
            if (y + 72 > H - 45) break;
            drawPanel(30, y, std::min(980, W - 60), 60, C(0x111922), C(0x283440));
            window_.drawText(50, y + 24, p.name, C(0xEDF2F6), 14);
            window_.drawText(50, y + 45, p.root.string(), C(0x718093), 12);
            drawButton(std::min(W - 142, 870), y + 13, 104, 34, "Open", true);
            y += 72;
        }
    }
    std::string today; if (account_.birthdayToday(today)) { drawPanel(30,H-70,std::min(560,W-60),34,C(0x1B2630),C(0x405A6D)); window_.drawText(42,H-48,"Happy Birthday, " + (account_.profile().displayName.empty()?account_.profile().email:account_.profile().displayName) + "!  " + today,C(0xD8E9F6),12); }
    window_.drawText(30, H - 20, "Local-first  |  Native C++20  |  AI + Build + Publish", C(0x526174), 11);
}

void ForgeEditor::paintCreate() {
    const int W = window_.width(), H = window_.height();
    window_.drawRect(0, 0, W, H, C(0x0B0F14), true);
    window_.drawText(38, 52, "CREATE PROJECT", C(0xF2F6FA), 23);
    window_.drawText(40, 78, "Configure a project before entering the editor.", C(0x7F8D9F), 14);

    const int cw = std::min(760, W - 80);
    drawPanel(40, 110, cw, 465, C(0x111922), C(0x2B3845));
    window_.drawText(66, 145, "PROJECT", C(0x95A8BB), 12);
    window_.drawText(66, 175, "Project Name", C(0xDDE7EF), 14);
    drawPanel(66, 187, cw - 52, 42, activeField_ == 0 ? C(0x1D2A36) : C(0x171F28), C(0x3A4A59));
    window_.drawText(80, 214, projectName_ + (activeField_ == 0 ? "|" : ""), C(0xF1F5F8), 14);

    window_.drawText(66, 258, "Project Location", C(0xDDE7EF), 14);
    drawPanel(66, 270, cw - 52, 42, activeField_ == 1 ? C(0x1D2A36) : C(0x171F28), C(0x3A4A59));
    window_.drawText(80, 297, projectLocation_ + (activeField_ == 1 ? "|" : ""), C(0xF1F5F8), 13);

    window_.drawText(66, 344, "Template", C(0xDDE7EF), 14);
    drawButton(66, 357, 120, 38, "3D", true); drawButton(195, 357, 120, 38, "2D"); drawButton(324, 357, 120, 38, "Empty"); drawButton(453, 357, 135, 38, "Multiplayer");

    window_.drawText(66, 431, "Target", C(0xDDE7EF), 14);
    window_.drawText(66, 456, "Windows", C(0xD5DEE7), 13);
    window_.drawText(180, 456, "Linux", C(0xD5DEE7), 13);
    window_.drawText(280, 456, "Android", C(0xD5DEE7), 13);

    drawButton(66, 500, 120, 38, "Cancel");
    drawButton(198, 500, 170, 38, "Create Project", true);
    window_.drawText(40, H - 22, status_, C(0x58687B), 11);
}

void ForgeEditor::paintEditor() {
    const int W = window_.width(), H = window_.height();
    const int top = 52;
    const int statusH = 24;
    const int bottomH = std::clamp(H / 4, 150, 220);
    const int contentBottom = H - bottomH - statusH;
    const int leftW = std::clamp(W / 6, 220, 270);
    const int rightW = std::clamp(W / 5, 290, 340);
    const int centerX = leftW;
    const int centerW = std::max(300, W - leftW - rightW);
    const int rightX = centerX + centerW;

    window_.drawRect(0, 0, W, H, C(0x0B0F14), true);
    window_.drawRect(0, 0, W, top, C(0x121922), true);
    window_.drawRect(0, top, leftW, contentBottom - top, C(0x10171F), true);
    window_.drawRect(centerX, top, centerW, contentBottom - top, C(0x0A0F14), true);
    window_.drawRect(rightX, top, rightW, contentBottom - top, C(0x10171F), true);
    window_.drawRect(0, contentBottom, W, bottomH, C(0x0F151C), true);
    window_.drawRect(0, H - statusH, W, statusH, C(0x0B1016), true);

    // Toolbar
    window_.drawText(18, 34, "FORGE", C(0xFFFFFF), 19);
    drawButton(88, 10, 70, 32, playing_ ? "■ Stop" : "▶ Play", playing_);
    drawButton(164, 10, 66, 32, "Save");
    drawButton(236, 10, 72, 32, "Build");
    drawButton(316, 10, 74, 32, "Doctor");
    drawButton(398, 10, 58, 32, "AI");
    drawButton(464, 10, 62, 32, "Tests");
    drawButton(532, 10, 82, 32, "Publish");
    window_.drawText(std::max(550, W - 370), 33, project_.name.empty() ? "No Project" : project_.name, C(0x91A2B5), 13);

    // Scene tree
    window_.drawText(16, top + 24, "SCENE", C(0xE0E8EF), 14);
    window_.drawText(leftW - 72, top + 24, "LOCAL", C(0x556678), 10);
    int y = top + 42;
    for (const auto& e : scene_.entities()) {
        if (y + 30 > contentBottom - 60) break;
        const uint32_t fill = e.selected ? C(0x1F4057) : C(0x111A23);
        drawPanel(10, y, leftW - 20, 28, fill, e.selected ? C(0x39627E) : C(0x202D39));
        window_.drawText(24, y + 19, (e.visible ? "○  " : "×  ") + e.name, e.selected ? C(0xF0F6FA) : C(0xC0CBD6), 12);
        y += 32;
    }
    drawButton(12, contentBottom - 48, (leftW - 30) / 2, 32, "+ Cube", false);
    drawButton(18 + (leftW - 30) / 2, contentBottom - 48, (leftW - 30) / 2, 32, "World", false);

    // Viewport header and separators
    window_.drawRect(centerX, top, centerW, 34, C(0x0E141B), true);
    window_.drawText(centerX + 14, top + 23, playing_ ? "GAME" : "SCENE", C(0xC9D5DF), 12);
    window_.drawText(centerX + 74, top + 23, "3D", C(0x6D8194), 12);
    window_.drawText(centerX + centerW - 165, top + 23, "Perspective", C(0x6D8194), 11);
    const int vy = top + 34;
    const int vh = contentBottom - vy;
    const int cx = centerX + centerW / 2;
    const int cy = vy + vh / 2 + 20;
    for (int i = -14; i <= 14; ++i) {
        window_.drawLine(cx + i * 32, vy + 12, cx + i * 32, vy + vh - 15, C(0x121A22));
        window_.drawLine(centerX + 12, cy + i * 28, centerX + centerW - 12, cy + i * 28, C(0x121A22));
    }
    window_.drawLine(centerX + 12, cy, centerX + centerW - 12, cy, C(0x20303D));
    window_.drawLine(cx, vy + 12, cx, vy + vh - 15, C(0x20303D));

    auto* e = scene_.selected();
    if (e) {
        auto verts = renderer_.cube(e->position, e->scale, cx, cy);
        const int edges[12][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
        for (auto& edge : edges) {
            auto a = verts[edge[0]]; auto b = verts[edge[1]];
            window_.drawLine(a.x, a.y, b.x, b.y, C(0x69C7F2), 2);
        }
        window_.drawText(centerX + 18, vy + vh - 20,
                         "Selected  " + e->name + "    Position  " + std::to_string(e->position.x) + ", " + std::to_string(e->position.y) + ", " + std::to_string(e->position.z), C(0x71879A), 11);
    } else {
        window_.drawText(centerX + 24, cy, "Select an entity to inspect it", C(0x57697B), 14);
    }

    // Inspector
    window_.drawText(rightX + 16, top + 24, "INSPECTOR", C(0xE0E8EF), 14);
    window_.drawText(rightX + rightW - 64, top + 24, "3D", C(0x556879), 10);
    window_.drawLine(rightX + 12, top + 38, rightX + rightW - 12, top + 38, C(0x273440));
    if (e) {
        window_.drawText(rightX + 16, top + 66, e->name, C(0xF1F5F8), 16);
        window_.drawText(rightX + 16, top + 90, "Entity / Transform", C(0x74869A), 11);
        window_.drawText(rightX + 16, top + 125, "TRANSFORM", C(0x93A5B6), 11);
        auto field = [&](int yy, const char* label, float value) {
            window_.drawText(rightX + 16, yy + 20, label, C(0x788A9B), 11);
            std::ostringstream os; os << std::fixed << std::setprecision(2) << value;
            drawPanel(rightX + 66, yy, rightW - 82, 28, C(0x171F28), C(0x30404E));
            window_.drawText(rightX + 78, yy + 20, os.str(), C(0xE6EDF2), 12);
        };
        field(top + 140, "X", e->position.x); field(top + 176, "Y", e->position.y); field(top + 212, "Z", e->position.z);
        field(top + 260, "SX", e->scale.x); field(top + 296, "SY", e->scale.y); field(top + 332, "SZ", e->scale.z);
        drawButton(rightX + 16, top + 382, rightW - 32, 32, "Delete Entity");
    } else {
        window_.drawText(rightX + 16, top + 66, "Nothing selected", C(0x8291A2), 14);
        window_.drawText(rightX + 16, top + 94, "Select an entity in the Scene panel.", C(0x657587), 12);
    }

    // Bottom workspace
    const int tabY = contentBottom;
    const char* tabs[] = {"Console", "Assets", "Debugger", "Profiler", "Network", "AI", "Security"};
    int tx = 14;
    for (int i = 0; i < 7; ++i) {
        const bool selected = (panel_ == Panel::Scene && i == 0) ||
                              (panel_ == Panel::Assets && i == 1) ||
                              (panel_ == Panel::Profiler && i == 3) ||
                              (panel_ == Panel::Network && i == 4) ||
                              (panel_ == Panel::AI && i == 5) ||
                              ((panel_ == Panel::Build || panel_ == Panel::Tests || panel_ == Panel::Doctor || panel_ == Panel::Systems) && i == 2);
        drawButton(tx, tabY + 10, 92, 28, tabs[i], selected); tx += 98;
    }
    if (panel_ == Panel::Assets) {
        window_.drawText(18, tabY + 58, "Indexed assets", C(0xA5B2BF), 12);
        window_.drawText(120, tabY + 58, std::to_string(assets_.assets().size()), C(0xE1E8EE), 12);
    } else if (panel_ == Panel::Profiler) {
        window_.drawText(18, tabY + 58, "Frame", C(0x8091A3), 11);
        window_.drawText(66, tabY + 58, std::to_string(profiler_.frameMs()) + " ms", C(0xDCE5EC), 12);
        window_.drawText(150, tabY + 58, "FPS", C(0x8091A3), 11);
        window_.drawText(182, tabY + 58, std::to_string(profiler_.fps()), C(0xDCE5EC), 12);
    } else if (panel_ == Panel::Network) {
        window_.drawText(18, tabY + 58, "Latency 60 ms   Jitter 8 ms   Loss 2%", C(0xAAB7C4), 12);
    } else if (panel_ == Panel::AI) {
        window_.drawText(18, tabY + 58, "Forge AI", C(0xAAB7C4), 12);
        window_.drawText(90, tabY + 58, "Offline provider · project-aware foundation", C(0x76889A), 11);
    } else if (panel_ == Panel::Security) {
        auto sr=security_.scan(project_.root); window_.drawText(18,tabY+58,sr.message,sr.secretsDetected?C(0xD59885):C(0x9DB2C0),11);
    } else {
        window_.drawText(18, tabY + 58, log_, C(0x9DADBD), 12);
        window_.drawText(18, tabY + 82, "F5 Play   Ctrl+S Save   F6 Build   F7 Doctor   F8 AI   F9 Tests   F10 Systems", C(0x526376), 11);
    }

    window_.drawText(12, H - 8, "FORGE ENGINE PROFESSIONAL 3.0  ·  Native Editor  ·  Local-first", C(0x4B5A6A), 10);
    window_.drawText(W - 170, H - 8, playing_ ? "PLAY MODE" : "EDITOR READY", playing_ ? C(0x7BC39B) : C(0x6B7E91), 10);
}

void ForgeEditor::runPublish() { publishProfile_.appName = project_.name; publishProfile_.publisher = account_.profile().displayName.empty()?"Forge Creator":account_.profile().displayName; if(publishProfile_.packageId.empty()) publishProfile_.packageId="com.forge."+project_.id.substr(0,8); auto plan=publisher_.validate(publishProfile_,project_.root); size_t errors=0; for(const auto& i:plan.issues) if(i.error) ++errors; screen_=ForgeScreen::Publish; panel_=Panel::Publish; setStatus(errors==0?"[PUBLISH] Validation ready":"[PUBLISH] "+std::to_string(errors)+" blocking issue(s)"); }
void ForgeEditor::submitLogin() { std::string e; if(account_.logIn(authEmail_,authPassword_,e)){screen_=ForgeScreen::Hub;authPassword_.clear();std::string d;if(account_.birthdayToday(d))setStatus("[ACCOUNT] Happy Birthday!  "+d);else setStatus("[ACCOUNT] Signed in");}else setStatus("[ACCOUNT] ERROR - "+e); }
void ForgeEditor::submitSignup() { account::AccountProfile p; p.displayName=authDisplay_;p.email=authEmail_;p.dateOfBirth=authDob_;p.country=authCountry_;p.timezone="Local";p.language=localization_.language();std::string e;if(account_.signUp(p,authPassword_,e)){screen_=ForgeScreen::Hub;authPassword_.clear();std::string d;if(account_.birthdayToday(d))setStatus("[ACCOUNT] Happy Birthday!  "+d);else setStatus("[ACCOUNT] Account created");}else setStatus("[ACCOUNT] ERROR - "+e); }
void ForgeEditor::paintPublish() { const int W=window_.width(),H=window_.height(); window_.drawRect(0,0,W,H,C(0x0B0F14),true); window_.drawRect(0,0,W,60,C(0x121922),true); window_.drawText(28,38,"FORGE PUBLISH CENTER",C(0xF2F6FA),22); const int x=42,cw=std::min(900,W-84); drawPanel(x,88,cw,H-140,C(0x111922),C(0x2B3845)); window_.drawText(x+24,124,"DESTINATION",C(0x9DB0C0),11); publish::Store stores[]={publish::Store::GooglePlay,publish::Store::AppleAppStore,publish::Store::MicrosoftStore,publish::Store::Steam,publish::Store::Direct}; int bx=x+24; for(int i=0;i<5;++i){drawButton(bx,140,142,34,publish::PublishCenter::storeName(stores[i]),publishProfile_.store==stores[i]);bx+=150;} auto field=[&](int yy,const char* label,const std::string& value){window_.drawText(x+24,yy,label,C(0xAEBCC8),11);drawPanel(x+24,yy+9,cw-48,38,C(0x171F28),C(0x30404C));window_.drawText(x+38,yy+34,value,C(0xE9EFF3),12);}; field(198,"App Name",publishProfile_.appName); field(252,"Publisher",publishProfile_.publisher); field(306,"Package / Bundle ID",publishProfile_.packageId); field(360,"Version",publishProfile_.version); field(414,"Privacy Policy URL",publishProfile_.privacyUrl.empty()?"<not configured>":publishProfile_.privacyUrl); auto plan=publisher_.validate(publishProfile_,project_.root); int yy=476; window_.drawText(x+24,yy,"VALIDATION",C(0x9DB0C0),11); for(size_t i=0;i<std::min<size_t>(6,plan.issues.size());++i){window_.drawText(x+24,yy+28+static_cast<int>(i)*24,(plan.issues[i].error?"ERROR  ":"WARN   ")+plan.issues[i].field+" - "+plan.issues[i].message,plan.issues[i].error?C(0xD59885):C(0xA6B7C4),10);} drawButton(x+24,H-102,100,36,"Back"); drawButton(x+138,H-102,150,36,"Validate",true); drawButton(x+300,H-102,120,36,"Build"); drawButton(x+432,H-102,120,36,"Upload"); window_.drawText(x+570,H-80,"Upload requires a connected developer account and platform approval.",C(0x637588),10); }

void ForgeEditor::createProject() {
    project::ProjectInfo p; std::string e; project::ProjectManager pm;
    auto base = std::filesystem::path(projectLocation_);
    if (!pm.createProject(projectName_, base, "3D", p, e)) { setStatus("[PROJECT] ERROR — " + e); return; }
    project_ = p; forgeAI_.setProject(project_.root); std::string se;
    if (!scene_.load(project_.root / project_.defaultScene, se)) { setStatus("[SCENE] ERROR — " + se); return; }
    assets_.scan(project_.root / "Assets", se); screen_ = ForgeScreen::Editor; panel_ = Panel::Scene;
    if (!scene_.entities().empty()) scene_.select(scene_.entities().front().id);
    setStatus("[PROJECT] Created real project · ready for editing");
}

void ForgeEditor::openProject() {
    std::string e; auto ps = project::ProjectManager().discoverProjects(project::ProjectManager::defaultProjectsDirectory());
    if (ps.empty()) { setStatus("[HUB] No projects found"); return; }
    project_ = ps.front(); forgeAI_.setProject(project_.root); std::string se; scene_.load(project_.root / project_.defaultScene, se); assets_.scan(project_.root / "Assets", se);
    screen_ = ForgeScreen::Editor; panel_ = Panel::Scene; if (!scene_.entities().empty()) scene_.select(scene_.entities().front().id);
    setStatus("[PROJECT] Opened " + project_.root.string());
}

void ForgeEditor::saveScene() { std::string e; if (scene_.save(project_.root / project_.defaultScene, e)) setStatus("[SCENE] Saved"); else setStatus("[SCENE] ERROR — " + e); }
void ForgeEditor::runBuild() { auto out = project_.root / "Builds" / "Development"; auto r = build::BuildCenter().build(project_.root, out, "Development"); setStatus(r.success ? "[BUILD] Success · " + std::to_string(r.files) + " files" : "[BUILD] ERROR — " + r.message); panel_ = Panel::Build; }
void ForgeEditor::runDoctor() { panel_ = Panel::Doctor; setStatus("[DOCTOR] Diagnostic scan ready"); }
void ForgeEditor::runAI() { panel_ = Panel::AI; auto r=forgeAI_.ask(aiplatform::Mode::Review,"review current project"); setStatus("[AI] " + r.text.substr(0, 96)); }
void ForgeEditor::runTests() { auto r = tests_.run(); size_t pass = 0; for (auto& t : r) if (t.passed) ++pass; panel_ = Panel::Tests; setStatus("[TEST] " + std::to_string(pass) + "/" + std::to_string(r.size()) + " passed"); }
void ForgeEditor::takeSnapshot() { std::string e; auto p = recovery_.snapshot(project_.root, "editor", e); setStatus(p.empty() ? "[RECOVERY] ERROR — " + e : "[RECOVERY] Snapshot created"); }
void ForgeEditor::generateWorld() { worldGen_.generate(scene_, "dark forest village"); saveScene(); setStatus("[WORLD] Editable procedural scene baseline generated"); }

void ForgeEditor::handleHubClick(int x, int y) {
    if (y > 228 && y < 286 && x < 230) { screen_ = ForgeScreen::CreateProject; activeField_ = 0; setStatus("[HUB] Ready"); }
    else if (y > 228 && y < 286 && x >= 230) openProject();
}

void ForgeEditor::handleCreateClick(int x, int y) {
    if (y > 180 && y < 240) activeField_ = 0;
    else if (y > 264 && y < 326) activeField_ = 1;
    else if (y > 490 && x < 190) screen_ = ForgeScreen::Hub;
    else if (y > 490 && x < 400) createProject();
}

void ForgeEditor::handleEditorClick(int x, int y) {
    const int H = window_.height(), W = window_.width();
    const int top = 52, statusH = 24, bottomH = std::clamp(H / 4, 150, 220), contentBottom = H - bottomH - statusH;
    const int leftW = std::clamp(W / 6, 220, 270), rightW = std::clamp(W / 5, 290, 340), centerW = std::max(300, W - leftW - rightW), rightX = leftW + centerW;
    if (y < top) {
        if (x > 80 && x < 160) { playing_ = !playing_; paused_ = false; setStatus(playing_ ? "[RUNTIME] Play started" : "[RUNTIME] Play stopped"); }
        else if (x >= 160 && x < 232) saveScene(); else if (x >= 232 && x < 312) runBuild(); else if (x >= 312 && x < 394) runDoctor(); else if (x >= 394 && x < 462) runAI(); else if (x >= 462 && x < 532) runTests(); else if (x >= 532 && x < 620) runPublish();
        return;
    }
    if (x < leftW) {
        if (y >= contentBottom - 60 && x < leftW / 2) { scene_.addCube(); setStatus("[SCENE] Cube added"); }
        else if (y >= contentBottom - 60) { generateWorld(); }
        else if (y >= top + 40) { int idx = (y - (top + 42)) / 32; if (idx >= 0 && idx < static_cast<int>(scene_.entities().size())) scene_.select(scene_.entities()[idx].id); }
    } else if (x >= rightX) {
        if (y > top + 360 && scene_.selected()) { scene_.removeSelected(); setStatus("[SCENE] Deleted selected entity"); }
    } else if (y >= contentBottom) {
        const int tab = (x - 14) / 96;
        if (tab == 1) panel_ = Panel::Assets; else if (tab == 3) panel_ = Panel::Profiler; else if (tab == 4) panel_ = Panel::Network; else if (tab == 5) panel_ = Panel::AI; else if (tab == 6) panel_ = Panel::Security; else panel_ = Panel::Scene;
    }
}

void ForgeEditor::handleChar(char32_t ch) {
    auto edit=[&](std::string& v,size_t max){if(ch==8){if(!v.empty())v.pop_back();}else if(ch>=32&&ch<127&&v.size()<max)v.push_back(static_cast<char>(ch));};
    if(screen_==ForgeScreen::CreateProject){if(activeField_==0)edit(projectName_,64);else edit(projectLocation_,240);}
    else if(screen_==ForgeScreen::Login){if(authField_==0)edit(authEmail_,160);else edit(authPassword_,128);}
    else if(screen_==ForgeScreen::Signup){if(authField_==0)edit(authDisplay_,64);else if(authField_==1)edit(authEmail_,160);else if(authField_==2)edit(authPassword_,128);else if(authField_==3)edit(authDob_,16);else edit(authCountry_,64);}
    window_.invalidate();
}
void ForgeEditor::handleAuthClick(int x,int y,bool signup){const int W=window_.width(),cw=std::min(560,W-80),bx=(W-cw)/2;if(signup){if(y>510&&y<560&&x>bx+34&&x<bx+224){submitSignup();return;}if(y>510&&y<560&&x>bx+238&&x<bx+380){screen_=ForgeScreen::Login;authField_=0;return;}int ys[]={210,270,330,390,450};for(int i=0;i<5;++i)if(y>ys[i]&&y<ys[i]+58){authField_=i;return;}}else{if(y>350&&y<400&&x>bx+34&&x<bx+224){submitLogin();return;}if(y>350&&y<400&&x>bx+238&&x<bx+420){screen_=ForgeScreen::Signup;authField_=0;return;}int ys[]={220,280};for(int i=0;i<2;++i)if(y>ys[i]&&y<ys[i]+58){authField_=i;return;}}window_.invalidate();}
void ForgeEditor::handlePublishClick(int x,int y){const int H=window_.height(),px=42;publish::Store stores[]={publish::Store::GooglePlay,publish::Store::AppleAppStore,publish::Store::MicrosoftStore,publish::Store::Steam,publish::Store::Direct};int bx=px+24;for(int i=0;i<5;++i){if(x>bx&&x<bx+142&&y>140&&y<174){publishProfile_.store=stores[i];runPublish();return;}bx+=150;}if(y>H-110){if(x<142){screen_=ForgeScreen::Editor;return;}if(x<300){runPublish();return;}if(x<430){std::string e;auto r=build::BuildCenter().build(project_.root,project_.root/"Builds"/"Release","Release");setStatus(r.success?"[PUBLISH] Release build created":"[PUBLISH] ERROR - "+r.message);return;}if(x<560){setStatus("[PUBLISH] Upload requires connected platform credentials");return;}}}

void ForgeEditor::handleKey(Key k) {
    if(k==Key::Escape){ if(screen_==ForgeScreen::Editor||screen_==ForgeScreen::Publish) screen_=ForgeScreen::Hub; else if(screen_==ForgeScreen::CreateProject) screen_=ForgeScreen::Hub; window_.invalidate(); return; }
    if(screen_==ForgeScreen::Login){ if(k==Key::Tab) authField_=authField_==0?1:0; else if(k==Key::Enter) submitLogin(); window_.invalidate(); return; }
    if(screen_==ForgeScreen::Signup){ if(k==Key::Tab) authField_=(authField_+1)%5; else if(k==Key::Enter) submitSignup(); window_.invalidate(); return; }
    if(screen_==ForgeScreen::CreateProject){ if(k==Key::Tab) activeField_=activeField_==0?1:0; else if(k==Key::Enter) createProject(); window_.invalidate(); return; }
    if(screen_==ForgeScreen::Publish){ if(k==Key::F6){ std::string e; auto r=build::BuildCenter().build(project_.root,project_.root/"Builds"/"Release","Release"); setStatus(r.success?"[PUBLISH] Release build created":"[PUBLISH] ERROR - "+r.message); } window_.invalidate(); return; }
    auto* e=scene_.selected(); const float step=0.25f;
    if(k==Key::F5||k==Key::Space){playing_=!playing_;setStatus(playing_?"[RUNTIME] Play started":"[RUNTIME] Play stopped");}
    else if(k==Key::F6) runBuild(); else if(k==Key::F7) runDoctor(); else if(k==Key::F8) runAI(); else if(k==Key::F9) runTests(); else if(k==Key::F10) runPublish();
    else if(k==Key::DeleteKey&&e){scene_.removeSelected();setStatus("[SCENE] Deleted selected entity");}
    else if(e){if(k==Key::Left)e->position.x-=step;else if(k==Key::Right)e->position.x+=step;else if(k==Key::Up)e->position.z-=step;else if(k==Key::Down)e->position.z+=step;else if(k==Key::W)e->position.y+=step;else if(k==Key::S)e->position.y-=step;else if(k==Key::N)panel_=Panel::Network;else if(k==Key::P)panel_=Panel::Profiler;else if(k==Key::A)panel_=Panel::Assets;else if(k==Key::R)takeSnapshot();}
    window_.invalidate();
}

void ForgeEditor::onEvent(const InputEvent& e) {
    if (e.type == InputEvent::Type::Close) { window_.requestClose(); return; }
    if (e.type == InputEvent::Type::MouseMove) { mouseX_ = e.x; mouseY_ = e.y; window_.invalidate(); return; }
    if (e.type == InputEvent::Type::KeyDown) { handleKey(e.key); return; }
    if (e.type == InputEvent::Type::KeyChar) { handleChar(e.character); return; }
    if (e.type == InputEvent::Type::MouseDown && e.mouseButton == MouseButton::Left) {
        if (screen_ == ForgeScreen::Login) handleAuthClick(e.x,e.y,false); else if (screen_ == ForgeScreen::Signup) handleAuthClick(e.x,e.y,true); else if (screen_ == ForgeScreen::Hub) handleHubClick(e.x,e.y); else if (screen_ == ForgeScreen::CreateProject) handleCreateClick(e.x,e.y); else if (screen_ == ForgeScreen::Publish) handlePublishClick(e.x,e.y); else handleEditorClick(e.x,e.y);
    }
}

} // namespace forge::editor
