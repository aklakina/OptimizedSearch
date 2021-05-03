#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QTableWidgetItem>
#include "net.h"
#include <thread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    void on_actionOpen_triggered();

    void on_actionSave_as_triggered();

    int on_tableWidget_itemChanged(QTableWidgetItem *item);

    int on_tableWidget_itemClicked(QTableWidgetItem *item);

    void on_actionCalc_next_step_triggered();

    void on_actionGenerate_Neural_Network_topology_triggered();

    void on_actionTeach_scheme_triggered(bool recent=false);

    void on_pushButton_clicked();

    void on_actionSave_Wieghts_file_triggered();

    void on_actionLoad_Weights_file_triggered();

    void on_actionGenerate_random_map_triggered();

    void on_actionStart_calc_triggered();

    void on_actionExit_triggered();

    void on_actionToggle_Teach_mode_toggled(bool arg1);

    void on_actionQuick_Search_triggered();

    void on_actionWill_I_Die_triggered();

    void on_actionShould_I_play_the_moves_too_toggled(bool arg1);

private:
    Ui::MainWindow *ui;
    int powerofrecursion(unsigned long long depth, QString type, QTableWidgetItem* item, int row=0, int col=0, std::vector<std::vector<std::pair<QString, bool> > > *storage=nullptr);
    bool threematched=false;
    bool morethanthreematched=false;
    int connections=0;
    bool loading=false;
    bool sequenceloading=false;
    std::vector<QString> sequence;
    int nextinsequence;
    net* brain=nullptr;
    bool shouldidoitagain=false;
    std::vector<unsigned> topology;
    int imgonnadoitagain(QTableWidgetItem* item=nullptr, int row=0, int col=0, std::vector<std::vector<std::pair<QString, bool> > > *storage=nullptr, std::vector<std::pair<int, int> > *usethisnull=nullptr);
    void tablereset(std::vector<std::vector<std::pair<QString,bool>> > backup);
    void FeedRandom(int row_num, int col_num);
    bool neural_projection=false;
    bool randomlygenerated=false;
    std::vector<std::vector<std::pair<QString,bool>>> full_copy;
    std::vector<double> input;
    std::vector<double> results;
    std::vector<double> corr;
    std::pair<std::vector<std::pair<int,int>>,int> values;
    std::vector<std::pair<std::vector<std::pair<int,int>>,int>> full_path;
    int recentaction;
    void on_actionGenerate_Neural_Network_topology_triggered_same_but_different(int row_num=0, int col_num=0);
    bool checkIfEnd();
    std::vector<std::pair<double,std::pair<int,int>>> target;
    unsigned hidden_layer_number=3;
    unsigned num_of_hidden_neurons=15;
    bool recursivesearch=false;
    bool teaching=false;
    bool isPlaying=false;
    bool tryagain=true;
    int depth=0;
    void NoMoreEventForYou();
    void GarbageCollector();
    void FillBackupVector(std::vector<std::vector<std::pair<QString,bool>> > &full_copy);
    std::vector<std::pair<unsigned,unsigned>> IDied;
    int StartCalc(unsigned depth, std::vector<std::pair<int, int> > *path=nullptr, double value=0, std::vector<std::vector<std::pair<QString, bool> > > *storage=nullptr, std::vector<std::pair<int, int> > *usethisnull=nullptr);
    std::vector<std::pair<double,std::pair<int,int>>> ProcessData(std::vector<std::pair<std::vector<std::pair<int,int>>,int>>* data);
    unsigned ForwardThinking=2;
    bool FeedSame=false;
    bool FoundOne=false;
    bool quick=false;
    int Neural_Calc(std::vector<std::vector<std::pair<QString,bool>>>* storage, int row, int col, std::vector<std::pair<int, int> > *usethisnull);
    void kalap(std::vector<std::vector<std::pair<int,int>>> alma);
    bool youwontdie=false;
    bool playing=false;
    bool INeedJustOne=false;
    unsigned savekey=0;
    int correctionalfunction(std::vector<std::vector<std::pair<QString,bool>>>* storage, int row, int col, std::vector<std::pair<int, int> > *usethisnull);
    std::vector<std::pair<int,int>> nullspaces; //első a row a második a column
};
#endif // MAINWINDOW_H
