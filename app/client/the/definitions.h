
/** these are our singletons, please, don't invent any more! */

class Settings;
class MainWindow;
class App;
class Radio;

namespace The
{
    MainWindow& mainWindow();
    Radio& radio();
    Settings& settings();
    App& app();
}
