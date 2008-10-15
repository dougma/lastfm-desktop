
/** these are our singletons, please, don't invent any more! */

class MooseConfig;
class MainWindow;
class App;
class Radio;

namespace The
{
    MainWindow& mainWindow();
    Radio& radio();
    MooseConfig& config();
    App& app();
}
