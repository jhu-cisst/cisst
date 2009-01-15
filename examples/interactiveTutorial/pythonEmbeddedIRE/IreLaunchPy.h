// Launch IRE in Python-created thread
class IreLaunch {
public:
    IreLaunch() {}
    ~IreLaunch() { ireFramework::FinalizeShell();}
    void Run(char *startup) {
        try {
            ireFramework::LaunchIREShell(startup, true);
        }
        catch (...) {
            cout << "*** ERROR:  could not launch IRE shell ***" << endl;
        }
    }
};
