#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    srand(time(NULL));
    randomlygenerated=true;
}

MainWindow::~MainWindow()
{
    delete ui;
    ui->label->setAlignment(Qt::AlignCenter);
}

void MainWindow::on_actionOpen_triggered()
{
    randomlygenerated=false;
    if (ui->tableWidget->rowCount()>0) {
        QMessageBox::StandardButton reply=QMessageBox::warning(this,"Table not empty","All your data will be lost if you continue. Do you want to save them?",QMessageBox::Yes | QMessageBox::Discard | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes){
            on_actionSave_as_triggered();
        } else if (reply==QMessageBox::Cancel){
            return;
        }
    }
    ui->tableWidget->clear();
    sequence.clear();
    nullspaces.clear();
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Load a levimap file",
                                                    ".");
    QFile alma(filename);
    if (!alma.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
    QString str1="";
    QTextStream in(&alma);
    /*str1=in.readAll();
    qDebug()<<str1;*/
    int k=0;
    while (!in.atEnd()) {
        str1=in.readLine();
        qDebug()<<str1;
        if (str1=="") {
            sequenceloading=true;
        }
        if (!sequenceloading) {
            ui->tableWidget->setColumnCount(str1.count(";")+1);
            for (int i=0;i<str1.count(";")+1;i++) {
                ui->tableWidget->setRowCount(k+1);
                NoMoreEventForYou();
                ui->tableWidget->setItem(k,i,new QTableWidgetItem());
                if (str1.section(";",i,i).count(",")!=0) {
                    NoMoreEventForYou();
                    ui->tableWidget->item(k,i)->setText(str1.section(";",i,i).section(",",0,0)+" "+str1.section(";",i,i).section(",",1,1));
                } else {
                    NoMoreEventForYou();
                    ui->tableWidget->item(k,i)->setText(str1.section(";",i,i));
                    NoMoreEventForYou();
                    ui->tableWidget->item(k,i)->setBackground(QBrush(QColor(0,100,20)));
                    nullspaces.push_back({k,i});
                }
                NoMoreEventForYou();
                ui->tableWidget->item(k,i)->setCheckState(Qt::Unchecked);
            }
            k++;
        } else if (sequenceloading && str1!="") {
            for (int i=0;i<str1.count(";")+1;i++) {
                sequence.push_back(str1.section(";",i,i).section(",",0,0)+" "+str1.section(";",i,i).section(",",1,1));
            }
            nextinsequence=0;
            ui->label->setText(sequence[nextinsequence]);
            sequenceloading=false;
        }
    }
    ui->spinBox->setValue(ui->tableWidget->columnCount());
    ui->spinBox_2->setValue(ui->tableWidget->rowCount());
}

void MainWindow::on_actionSave_as_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Choose a levimap file to save",
                                                    ".");
    QFile alma(filename);
    if (!alma.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
    QString str1="";
    //int whole=ui->tableWidget->rowCount()*ui->tableWidget->columnCount();
    for (int k=0;k<ui->tableWidget->rowCount();k++) {
        for (int i=0;i<ui->tableWidget->columnCount();i++) {
            if (ui->tableWidget->item(k,i)->text()=="null") {
                str1+=ui->tableWidget->item(k,i)->text();
            } else {
                str1+=ui->tableWidget->item(k,i)->text().section(" ",0,0)+","+ui->tableWidget->item(k,i)->text().section(" ",1,1);
            }
            if (i!=ui->tableWidget->columnCount()-1) {
                str1+=";";
            }
        }
        str1+='\n';
    }
    str1+='\n';
    for (unsigned i=0;i<sequence.size();i++) {
        str1+=sequence[i].section(" ",0,0)+","+sequence[i].section(" ",1,1);
        if (sequence.size()-1!=i) {
            str1+=";";
        }
    }
    QTextStream out(&alma);
    out<<str1;
}

int MainWindow::powerofrecursion(unsigned long long depth, QString type,QTableWidgetItem* item,int row, int col,std::vector<std::vector<std::pair<QString,bool>>>* storage) {
    if ((item!=nullptr && (item->checkState()==Qt::Checked || type!=item->text() || item->text()=="null")) || (neural_projection && (type!=storage->at(col).at(row).first || storage->at(col).at(row).second)) ) {
        return 0;
    }
    if (depth>3000) {
        IDied.push_back({item->row(),item->column()});
        return 0;
    }
    if (!neural_projection) {
        NoMoreEventForYou();
        item->setCheckState(Qt::Checked);
        if (item->row()<ui->tableWidget->rowCount()-1)
            powerofrecursion(depth+1,item->text(),ui->tableWidget->item(item->row()+1,item->column()));
        if (item->row()>0)
            powerofrecursion(depth+1,item->text(),ui->tableWidget->item(item->row()-1,item->column()));
        if (item->column()<ui->tableWidget->columnCount()-1)
            powerofrecursion(depth+1,item->text(),ui->tableWidget->item(item->row(),item->column()+1));
        if (item->column()>0)
            powerofrecursion(depth+1,item->text(),ui->tableWidget->item(item->row(),item->column()-1));
    } else {
        storage->at(col).at(row).second=true;
        if (row<ui->tableWidget->rowCount()-1)
            powerofrecursion(depth+1,storage->at(col).at(row).first,nullptr,row+1,col,storage);
        if (row>0)
            powerofrecursion(depth+1,storage->at(col).at(row).first,nullptr,row-1,col,storage);
        if (col<ui->tableWidget->columnCount()-1)
            powerofrecursion(depth+1,storage->at(col).at(row).first,nullptr,row,col+1,storage);
        if (col>0)
            powerofrecursion(depth+1,storage->at(col).at(row).first,nullptr,row,col-1,storage);
    }
    return 0;
}

int MainWindow::on_tableWidget_itemChanged(QTableWidgetItem *item)
{
    int temp=0;
    if (!loading) {
        powerofrecursion(0,item->text(),item);
        int row=0;
        int col=0;
        while (IDied.size()>0) {
            row=IDied.back().first;
            col=IDied.back().second;
            IDied.pop_back();
            powerofrecursion(0,item->text(),ui->tableWidget->item(row,col));
        }
        for (auto row=0;row<ui->tableWidget->rowCount();row++) {
            for (auto column=0;column<ui->tableWidget->columnCount();column++) {
                if (ui->tableWidget->item(row,column)->checkState()==Qt::Checked && !morethanthreematched) {
                    connections++;
                    if (connections==3) {
                        threematched=true;
                    } else if (connections==4) {
                        morethanthreematched=true;
                        row=0;
                        column=0;
                        connections=0;
                    }
                } else if (ui->tableWidget->item(row,column)->checkState()==Qt::Checked && morethanthreematched && (item!=ui->tableWidget->item(row,column))) {
                    connections++;
                    NoMoreEventForYou();
                    ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
                    NoMoreEventForYou();
                    ui->tableWidget->item(row,column)->setText("null");
                    nullspaces.push_back({row,column});
                    NoMoreEventForYou();
                    ui->tableWidget->item(row,column)->setBackground(QBrush(QColor(0,100,20)));
                }
            }
        }
        if (threematched) {
            for (auto row=0;row<ui->tableWidget->rowCount();row++) {
                for (auto column=0;column<ui->tableWidget->columnCount();column++) {
                    if (ui->tableWidget->item(row,column)->checkState()==Qt::Checked && (item!=ui->tableWidget->item(row,column))) {
                        NoMoreEventForYou();
                        ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
                        NoMoreEventForYou();
                        ui->tableWidget->item(row,column)->setText("null");
                        nullspaces.push_back({row,column});
                        NoMoreEventForYou();
                        ui->tableWidget->item(row,column)->setBackground(QBrush(QColor(0,100,20)));
                    }
                }
            }
            NoMoreEventForYou();
            item->setText(item->text().section(" ",0,0)+" "+QString::number(item->text().section(" ",1,1).toInt()+1));
            shouldidoitagain=true;
        } else {
            for (auto row=0;row<ui->tableWidget->rowCount();row++) {
                for (auto column=0;column<ui->tableWidget->columnCount();column++) {
                    if (ui->tableWidget->item(row,column)->checkState()==Qt::Checked && (item!=ui->tableWidget->item(row,column))) {
                        NoMoreEventForYou();
                        ui->tableWidget->item(row,column)->setCheckState(Qt::Unchecked);
                        connections=0;
                    }
                }
            }
        }
        morethanthreematched=false;
        threematched=false;
        temp=connections;
        connections=0;
        NoMoreEventForYou();
        item->setCheckState(Qt::Unchecked);
        NoMoreEventForYou();
        item->setBackground(QBrush(QColor(255,255,255)));
    } else {
        loading=false;
    }
    if (shouldidoitagain) {
        shouldidoitagain=false;
        temp+=imgonnadoitagain(item);
    }
    return temp;
}

int MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    if (item->text()=="null") {
        NoMoreEventForYou();
        if (nullspaces.size()==1) {
            nullspaces.clear();
        } else {
            for (auto it=nullspaces.begin();it!=nullspaces.end();it++) {
                if (it->first==item->row() && it->second==item->column()) {
                    nullspaces.erase(it);
                }
            }
        }
        item->setText(sequence[nextinsequence]);
        if (!neural_projection) {
            if (unsigned(nextinsequence)==sequence.size()-1) {
                nextinsequence=0;
            } else {
                nextinsequence++;
            }
            ui->label->setText(sequence[nextinsequence]);
        }
        int temp=on_tableWidget_itemChanged(item);
        /*if (checkIfEnd()) {
            QMessageBox::warning(this,"Defeat","You are out of steps. Better luck next time.");
            temp=-500;
        }*/
        return temp;
    } else {
        return -1;
    }
}

bool MainWindow::checkIfEnd() {
    for (int row=0;row<ui->tableWidget->rowCount();row++) {
        for (int col=0;col<ui->tableWidget->columnCount();col++) {
            if (ui->tableWidget->item(row,col)->text()=="null") {
                return false;
            }
        }
    }
    return true;
}

void MainWindow::FillBackupVector(std::vector<std::vector<std::pair<QString, bool> > > &full_copy) {
    std::vector<std::pair<QString,bool>> alma;
    for (auto col=0;col<ui->tableWidget->columnCount();col++) {
        for (auto row=0;row<ui->tableWidget->rowCount();row++) {
            alma.push_back({ui->tableWidget->item(row,col)->text(),false});
        }
        full_copy.push_back(alma);
        alma.clear();
    }
}

int MainWindow::StartCalc(unsigned depth, std::vector<std::pair<int,int>> *path, double value, std::vector<std::vector<std::pair<QString,bool>>> *storage, std::vector<std::pair<int, int> > *usethisnull) {
    if ((quick && FoundOne) || youwontdie)
        return 0;
    if (depth==ForwardThinking && INeedJustOne) {
        youwontdie=true;
        qDebug()<<"A solution is: ";
        for (auto string:*path) {
            qDebug()<<"in row: "+QString::number(string.first)+" in col: "+QString::number(string.second)+'\n';
        }
        if (playing) {
            nullspaces=*usethisnull;
            tablereset(*storage);
            savekey=nextinsequence;
        }
        return 0;
    }
    std::vector<std::pair<int,int>> seged1;
    if (path!=nullptr) {
        if (path->back().first==-1) {
            return 0;
        }
    }
    if (usethisnull->size()==0)
        return 0;
    /*unsigned index;
    if (depth!=0 && path!=nullptr) {
        for (unsigned i=0;i<full_path.size();i++) {
            if (full_path[i].first==*path) {
                index=i;
                break;
            }
        }
    }*/
    double temp=0;
    for (auto it=usethisnull->begin();it!=usethisnull->end();it++) {
        std::vector<std::vector<std::pair<QString,bool>>> foraction=*storage;
        std::vector<std::pair<int,int>> foraction2=*usethisnull;
        if (usethisnull->size()==0)
            return 0;
        temp=correctionalfunction(&(foraction),it->first,it->second,&foraction2);
        if (foraction2.size()==0 && depth!=ForwardThinking-1)
            return 0;
        //if (temp!=-1) {
        if (quick && temp+value>=3) {
            FoundOne=true;
        }
        //}
        if (path!=nullptr)
            seged1=*path;
        seged1.push_back({it->first,it->second});
        if (/*temp!=-1 && */depth!=ForwardThinking-1 && foraction2.size()>0) {
            if (quick) {
                if (unsigned(nextinsequence)==sequence.size()-1) {
                    nextinsequence=0;
                } else {
                    nextinsequence++;
                }

                StartCalc(depth+1,&seged1,temp+value,&foraction,&foraction2);
                if (unsigned(nextinsequence)==0) {
                    nextinsequence=sequence.size()-1;
                } else {
                    nextinsequence--;
                }
            } else {
                if (unsigned(nextinsequence)==sequence.size()-1) {
                    nextinsequence=0;
                } else {
                    nextinsequence++;
                }
                StartCalc(depth+1,&seged1,0,&foraction,&foraction2);
                if (youwontdie)
                    return 0;
                if (unsigned(nextinsequence)==0) {
                    nextinsequence=sequence.size()-1;
                } else {
                    nextinsequence--;
                }
                if (INeedJustOne && youwontdie) {
                    return 0;
                }
            }
            if (!INeedJustOne)
                full_path.back().second+=temp;
        } else if (depth==ForwardThinking-1) {
                if (INeedJustOne) {
                    if (unsigned(nextinsequence)==sequence.size()-1) {
                        nextinsequence=0;
                    } else {
                        nextinsequence++;
                    }
                    StartCalc(depth+1,&seged1,0,&foraction,&foraction2);
                    if (youwontdie)
                        return 0;
                    if (unsigned(nextinsequence)==0) {
                        nextinsequence=sequence.size()-1;
                    } else {
                        nextinsequence--;
                    }
                } else {
                    seged1=*path;
                    seged1.push_back({it->first,it->second});
                    full_path.push_back({seged1,temp});
                }
        } else if (depth==0 && !INeedJustOne) {
            seged1.push_back({it->first,it->second});
            full_path.push_back({seged1,-1});
        } else if (temp==-1 && depth!=0) {
            continue;
        }
        seged1.clear();
        if (FoundOne)
            return 0;
    }
    return temp;
}

int MainWindow::correctionalfunction(std::vector<std::vector<std::pair<QString,bool>>>* storage, int row, int col, std::vector<std::pair<int,int>> *usethisnull) {
    if (storage->at(col).at(row).first!="null") {
        return -1;
    } else {
        storage->at(col).at(row).first=sequence[nextinsequence];
        if (usethisnull->size()>1) {
            for (auto it=usethisnull->begin();it!=usethisnull->end();it++) {
                if (it->first==row && it->second==col) {
                    usethisnull->erase(it);
                    break;
                }
            }
        } else {
            usethisnull->clear();
        }
        return Neural_Calc(storage,row,col,usethisnull);
    }
}

std::vector<std::pair<double,std::pair<int,int>>> MainWindow::ProcessData(std::vector<std::pair<std::vector<std::pair<int,int>>,int>> *data) {
    std::vector<std::pair<double,std::pair<int,int>>> temp;
    std::pair<int,int> compare;
    bool ifound=false;
    for (int row=0;row<ui->tableWidget->rowCount();row++) {
        for (int col=0;col<ui->tableWidget->columnCount();col++) {
            compare={row,col};
            for (auto it:*data) {
                if (it.first[0].first==compare.first && it.first[0].second==compare.second) {
                    for (unsigned i=0;i<temp.size();i++) {
                        if (temp[i].second.first==it.first[0].first && temp[i].second.second==it.first[0].second) {
                            if (temp[i].first<it.second) {
                                temp[i].first=it.second;
                                ifound=true;
                                break;
                            }
                        }
                    }
                    if (!ifound) {
                        temp.push_back({it.second,it.first[0]});
                    } else {
                        ifound=false;
                    }
                }
            }
        }
    }
    return temp;
}

void MainWindow::on_actionCalc_next_step_triggered()
{
    recentaction=0;
    neural_projection=true;
    qDebug()<<"Starting Calculations";
    QString type=sequence[nextinsequence];
    int szabadhelyek=0;
    int osszeshely=ui->tableWidget->rowCount()*ui->tableWidget->columnCount();
    FillBackupVector(full_copy);
    StartCalc(0,nullptr,0,&full_copy,&nullspaces);
    double temp=0;
    for (unsigned row=0;row<full_copy[0].size();row++) {
        for (unsigned col=0;col<full_copy.size();col++) {
            if (ui->tableWidget->item(row,col)->text()=="null") {
                temp=double(0);
            } else {
                temp=ui->tableWidget->item(row,col)->text().section(" ",0,0).toDouble()+ui->tableWidget->item(row,col)->text().section(" ",1,1).toDouble()/1000;
            }
            input.push_back(temp);
            if (full_copy[col][row].first=="null") {
                szabadhelyek++;
            }
        }
    }
    target=ProcessData(&full_path);
    input.push_back(sequence[nextinsequence].section(" ",0,0).toDouble()+sequence[nextinsequence].section(" ",1,1).toDouble()/1000);
    qDebug()<<"Copied table and calculated possible inputs";
    double maxElementValue_input=0;
    int maxElementIndex_input=0;
    for (unsigned i=0;i<target.size();i++) {
        if (target[i].first>maxElementValue_input) {
            maxElementValue_input=target[i].first;
            maxElementIndex_input=i;
        }
    }
    qDebug()<<QString::number(target[maxElementIndex_input].second.first)+" "+QString::number(target[maxElementIndex_input].second.second)+" "+QString::number(maxElementValue_input);
    if (recursivesearch) {
        recursivesearch=false;
        NoMoreEventForYou();
        ui->tableWidget->item(target[maxElementIndex_input].second.first,target[maxElementIndex_input].second.second)->setBackground(QBrush(QColor(200,0,0)));
        neural_projection=false;
        GarbageCollector();
        return;
    }
    brain->feedForward(input);
    brain->getResults(results);
    unsigned maxElementIndex=0;
    for (unsigned i=0; i<results.size();i++) {
        if (results[i]>results[maxElementIndex]) {
            maxElementIndex=i;
        }
    }
    NoMoreEventForYou();
    ui->tableWidget->item(floor(maxElementIndex/(ui->tableWidget->columnCount())),floor(maxElementIndex%(ui->tableWidget->columnCount())))->setBackground(QBrush(QColor(200,0,0)));
    if (teaching) {
        for (auto it:target) {
            if (it.first==-1) {
                corr.push_back(0);
            } else {
                corr.push_back(double((it.first+szabadhelyek)/osszeshely));
            }
        }
        brain->backProp(corr);
    }
    qDebug()<<"Calculated neural network output, You should place the next item at row number: "+QString::number((maxElementIndex/ui->tableWidget->columnCount())+1)
              +" and column number: "+QString::number((maxElementIndex%ui->tableWidget->columnCount())+1)+" You will free up "+QString::number(target[maxElementIndex].first-1)+" number of tile.";
    neural_projection=false;
    if (!FeedSame)
        GarbageCollector();
}

void MainWindow::GarbageCollector() {
    input.clear();
    results.clear();
    target.clear();
    full_copy.clear();
    full_path.clear();
    values.first.clear();
    values.second=0;
}

void MainWindow::tablereset(std::vector<std::vector<std::pair<QString, bool> > > backup) {
    for (int i=0;i<ui->tableWidget->rowCount();i++) {
        for (int k=0;k<ui->tableWidget->columnCount();k++) {
            delete ui->tableWidget->item(i,k);
        }
    }
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(backup[0].size());
    ui->tableWidget->setColumnCount(backup.size());
    for (unsigned col=0; col<backup.size();col++) {
        for (unsigned row=0; row<backup[col].size();row++) {
            NoMoreEventForYou();
            ui->tableWidget->setItem(row,col,new QTableWidgetItem());
            NoMoreEventForYou();
            ui->tableWidget->item(row,col)->setText(backup[col][row].first);
            NoMoreEventForYou();
            ui->tableWidget->item(row,col)->setCheckState(Qt::Unchecked);
            if (backup[col][row].first=="null") {
                NoMoreEventForYou();
                ui->tableWidget->item(row,col)->setBackground(QBrush(QColor(0,100,20)));
            }
        }
    }
}

void MainWindow::on_actionGenerate_Neural_Network_topology_triggered()
{
    if (brain!=nullptr) {
        QMessageBox::StandardButton reply= QMessageBox::warning(this,"Brain already exists","By this action the Brain will be regenerated. Do you wish to proceed?", QMessageBox::Yes | QMessageBox::No);
        if (reply==QMessageBox::Yes) {
            brain->clear();
        } else {
            return;
        }
    }
    topology.push_back(ui->tableWidget->rowCount()*ui->tableWidget->columnCount()+1);
    for (unsigned i=0; i<hidden_layer_number;i++) {
        topology.push_back(num_of_hidden_neurons);
    }
    topology.push_back(ui->tableWidget->rowCount()*ui->tableWidget->columnCount());
    brain=new net(topology);
}

int MainWindow::imgonnadoitagain(QTableWidgetItem* item,int row,int col,std::vector<std::vector<std::pair<QString,bool>>> *storage, std::vector<std::pair<int,int>>* usethisnull) {
    if (!neural_projection) {
        return on_tableWidget_itemChanged(item);
    } else {
        return Neural_Calc(storage,row,col,usethisnull);
    }
}



void MainWindow::on_actionTeach_scheme_triggered(bool recent)
{
    bool ize=teaching;
    teaching=true;
    if (brain==nullptr) {
        QMessageBox::warning(this,"No neural network found", "Error. First you need to generate a neural network over a map.");
        return;
    }
    if (!recent) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Teaching method", "Do you want to feed random data for the neural network? (Answering no will feed your loaded map over and over again)",
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes) {
            FeedRandom(ui->tableWidget->rowCount(),ui->tableWidget->columnCount());
        } else if (reply==QMessageBox::No) {
            FeedSame=true;
            on_actionCalc_next_step_triggered();
            FeedSame=false;
            for (int i=0;i<13000;i++) {
                brain->feedForward(input);
                results.clear();
                brain->getResults(results);
                unsigned maxElementIndex=0;
                for (unsigned i=0; i<results.size();i++) {
                    if (results[i]>results[maxElementIndex]) {
                        maxElementIndex=i;
                    }
                }
                NoMoreEventForYou();
                ui->tableWidget->item(floor(maxElementIndex/(ui->tableWidget->columnCount())),floor(maxElementIndex%(ui->tableWidget->columnCount())))->setBackground(QBrush(QColor(200,0,0)));
                brain->backProp(corr);
            }
            GarbageCollector();
        } else {
            return;
        }
    } else {
        FeedRandom(ui->tableWidget->rowCount(),ui->tableWidget->columnCount());
    }
    if (!ize)
        teaching=false;
}

void MainWindow::FeedRandom(int row_num, int col_num) {
    recentaction=1;
    if (!(ui->spinBox->value()==col_num && ui->spinBox_2->value()==row_num)) {
        on_actionGenerate_Neural_Network_topology_triggered_same_but_different(ui->spinBox->value(),ui->spinBox_2->value());
    }
    if (!randomlygenerated) {
        on_actionSave_as_triggered();
        randomlygenerated=true;
    }
    for (int k=0; k<13000;k++) {
        on_actionGenerate_random_map_triggered();
        on_actionCalc_next_step_triggered();   //ezt dolgozzuk át
    }
    ui->label->setText(sequence[nextinsequence]);
}

void MainWindow::on_pushButton_clicked()
{
    if (recentaction==0) {
        on_actionCalc_next_step_triggered();
    } else if (recentaction==1) {
        on_actionTeach_scheme_triggered(true);
    }
}

void MainWindow::on_actionSave_Wieghts_file_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Choose an nwd file to save weight data to",
                                                    ".");
    brain->saveWeightTemplate(filename);
}

void MainWindow::on_actionLoad_Weights_file_triggered()
{
    if (brain!=nullptr) {
        QMessageBox::StandardButton reply=QMessageBox::warning(this,"Brain already exists","Brain will be deleted and regenerated from load file. Do you want to save the weights?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply == QMessageBox::Yes){
            on_actionSave_Wieghts_file_triggered();
        } else if (reply==QMessageBox::Cancel){
            return;
        }
    }
    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Load an nwd file",
                                                    ".");
    brain=net::loadWeightTemplate(filename);
    if (brain==nullptr) {
        QMessageBox::critical(this,"Error opening file","Error during opening the given file, Object has not been created.");
    }
}

void MainWindow::on_actionGenerate_random_map_triggered()
{
    if (!randomlygenerated) {
        on_actionSave_as_triggered();
        randomlygenerated=true;
    }
    for (int i=0;i<ui->tableWidget->rowCount();i++) {
        for (int k=0;k<ui->tableWidget->columnCount();k++) {
            delete ui->tableWidget->item(i,k);
        }
    }
    ui->tableWidget->clear();
    nullspaces.clear();
    ui->tableWidget->setRowCount(ui->spinBox_2->value());
    ui->tableWidget->setColumnCount(ui->spinBox->value());
    sequence.clear();
    for (int row=0; row<ui->spinBox_2->value();row++) {
        for (int col=0;col<ui->spinBox->value();col++) {
            QString type=QString::number(rand()%5);
            if (rand()%3==2) {
                type="0";
                nullspaces.push_back({row,col});
            }
            if (type=="0") {
                type="null";
            } else {
                type+=" 1";
            }
            NoMoreEventForYou();
            ui->tableWidget->setItem(row,col,new QTableWidgetItem());
            NoMoreEventForYou();
            ui->tableWidget->item(row,col)->setText(type);
            if (type=="null") {
                NoMoreEventForYou();
                ui->tableWidget->item(row,col)->setBackground(QBrush(QColor(0,100,20)));
            }
        }
    }
    int limit=rand()%((ui->spinBox->value()*ui->spinBox_2->value())/3-5)+5;
    for (int i=0; i<limit;i++) {
        QString type=QString::number(rand()%(5-1)+1);
        type+=" 1";
        sequence.push_back(type);
    }
    nextinsequence=0;
    ui->label->setText(sequence[nextinsequence]);
}

void MainWindow::on_actionGenerate_Neural_Network_topology_triggered_same_but_different(int row_num,int col_num)
{
    brain->clear();
    if (row_num==0 || col_num==0) {
        QMessageBox::warning(this,"Wrong dimensions","Could not properly create neural network topology for this map dimension. Using old map dimension.");
        row_num=ui->tableWidget->rowCount();
        col_num=ui->tableWidget->columnCount();
    }
    topology.push_back(row_num*col_num+1);
    for (int i=0; i<2;i++) {
        topology.push_back(row_num*col_num/10);
    }
    topology.push_back(row_num*col_num);
    brain=new net(topology);
}

void MainWindow::on_actionStart_calc_triggered()
{
    recursivesearch=true;
    on_actionCalc_next_step_triggered();
}

void MainWindow::on_actionExit_triggered()
{
    if (!randomlygenerated) {
        QMessageBox::StandardButton reply=QMessageBox::warning(this,"Data loss", "This is not a randomly generated map. Do you want to save it?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (reply==QMessageBox::Yes) {
            on_actionSave_as_triggered();
            if (brain!=nullptr) {
                on_actionSave_Wieghts_file_triggered();
            }
        } else if (reply==QMessageBox::Cancel) {
            return;
        }
    }
    this->close();
}

void MainWindow::on_actionToggle_Teach_mode_toggled(bool arg1)
{
    teaching=arg1;
}

void MainWindow::NoMoreEventForYou() {
    loading=true;
}

void MainWindow::on_actionQuick_Search_triggered()
{
    quick=true;
    neural_projection=true;
    FillBackupVector(full_copy);
    StartCalc(0,nullptr,0,&full_copy,&nullspaces);
    NoMoreEventForYou();
    ui->tableWidget->item(full_path.back().first.back().first,full_path.back().first.back().second)->setBackground(QBrush(QColor(200,0,0)));
    quick=false;
    neural_projection=false;
    FoundOne=false;
    GarbageCollector();
}

int MainWindow::Neural_Calc(std::vector<std::vector<std::pair<QString,bool>>>* storage, int row, int col, std::vector<std::pair<int,int>>* usethisnull) {
    int temp=0;
    powerofrecursion(0,storage->at(col).at(row).first,nullptr,row,col,storage);
    int temprow=0,tempcol=0;
    while (IDied.size()>0) {
        temprow=IDied.back().first;
        tempcol=IDied.back().second;
        IDied.pop_back();
        powerofrecursion(0,storage->at(col).at(row).first,nullptr,temprow,tempcol,storage);
    }
    for (auto inrow=0;inrow<ui->tableWidget->rowCount();inrow++) {
        for (auto column=0;column<ui->tableWidget->columnCount();column++) {
            if (storage->at(column).at(inrow).second && !morethanthreematched) {
                connections++;
                temp++;
                if (connections==3) {
                    threematched=true;
                } else if (connections==4) {
                    morethanthreematched=true;
                }
            }
        }
    }
    if (threematched) {
        for (auto inrow=0;inrow<ui->tableWidget->rowCount();inrow++) {
            for (auto column=0;column<ui->tableWidget->columnCount();column++) {
                if (storage->at(column).at(inrow).second && !(inrow==row && column==col)) {
                    storage->at(column).at(inrow).second=false;
                    storage->at(column).at(inrow).first="null";
                    usethisnull->push_back({inrow,column});
                }
            }
        }
        storage->at(col).at(row).first=(storage->at(col).at(row).first.section(" ",0,0)+" "+QString::number(storage->at(col).at(row).first.section(" ",1,1).toInt()+1));
        storage->at(col).at(row).second=false;
        shouldidoitagain=true;
    } else {
        for (auto inrow=0;inrow<ui->tableWidget->rowCount();inrow++) {
            for (auto column=0;column<ui->tableWidget->columnCount();column++) {
                storage->at(column).at(inrow).second=false;
                connections=0;
            }
        }
    }
    morethanthreematched=false;
    threematched=false;
    storage->at(col).at(row).second=false;
    //temp=connections;
    connections=0;
    if (shouldidoitagain) {
        shouldidoitagain=false;
        temp+=imgonnadoitagain(nullptr,row,col,storage,usethisnull);
    }
    return temp;
}

void MainWindow::on_actionWill_I_Die_triggered()
{
    neural_projection=true;
    int temp=ForwardThinking;
    if (ui->spinBox_3->value()!=0)
        ForwardThinking=ui->spinBox_3->value()-1;
    FillBackupVector(full_copy);
    INeedJustOne=true;
    StartCalc(0,nullptr,0,&full_copy,&nullspaces);
    INeedJustOne=false;
    if (youwontdie) {
        QMessageBox::information(this,"Hurray!","Thankfully you won't die in "+QString::number(ForwardThinking));
        nextinsequence=savekey;
    } else {
        QMessageBox::critical(this,"Oops","Git gut (You will die in the given number of moves)");
    }
    youwontdie=false;
    ForwardThinking=temp;
    GarbageCollector();
    neural_projection=false;
}

void MainWindow::on_actionShould_I_play_the_moves_too_toggled(bool arg1)
{
    playing=arg1;
}
