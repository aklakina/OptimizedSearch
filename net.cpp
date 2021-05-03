#include "net.h"
#include <cassert>
#include <cmath>
#include <QDebug>
#include <QFile>
#include <QFileDialog>

net::net(const std::vector<unsigned> _topology,std::vector<std::vector<std::vector<std::pair<double,double>>>> *loaded_data)
{    
    Neuron::map_size=_topology.back();
    unsigned numlayers=_topology.size();
    for (unsigned i=0;i<numlayers;++i) {
        //qDebug()<<"I will now create the first layer";
        neurons.push_back(*(new std::vector<Neuron*>));
        for (unsigned k=0;k<=_topology[i];++k) {
            neurons.back().push_back(new Neuron(0));
        }
    }
    for (unsigned i=0;i<_topology.size();++i) {
        unsigned outputnum;
        if (i==_topology.size()-1) {
            outputnum=0;
        }else {
            outputnum=neurons[i+1].size();
        }
        for (unsigned k=0;k<=_topology[i];++k) {
            neurons[i][k]->setOutputNumber(outputnum);
            if (loaded_data!=nullptr && i<_topology.size()-1) {
                std::vector<std::vector<std::vector<std::pair<double,double>>>> temp=*loaded_data;
                neurons[i][k]->setOutputWeight(temp[i][k]);
            }
        }
        neurons.back().back()->setOutput(1.0);
    }
}

void net::feedForward(const std::vector<double> &inputVals) {
    //qDebug()<<QString::number(inputVals.size())+" "+QString::number(neurons[0].size());
    assert(inputVals.size() == neurons[0].size()-1);
    for (unsigned i=0; i<inputVals.size();++i) {
        neurons[0][i]->setOutput(inputVals[i]);
    }
    for (unsigned i=1;i<neurons.size();++i) {
        std::vector<Neuron*> prev=neurons[i-1];
        for (unsigned n=0; n<neurons[i].size(); ++n) {
            neurons[i][n]->feedForward(prev,n);
        }
    }
}

void net::backProp(const std::vector<double> &Target) {
    std::vector<Neuron*> outputlayer=neurons.back();
    error=0;

    for (unsigned i=0; i<outputlayer.size();++i) {
        double delta=Target[i]-outputlayer[i]->getOutput();
        error+=delta*delta;
    }
    error/=outputlayer.size()-1;
    error=sqrt(error);

    recentaverageerror=(recentaverageerror+smoothing+error)/(smoothing+1);
    for (unsigned i=0; i<outputlayer.size()-1;++i) {
        outputlayer[i]->calcoutputgrad(Target[i]);
    }
    for (int i=neurons.size()-2;i>-1;--i) {
        std::vector<Neuron*> hidden=neurons[i];
        std::vector<Neuron*> nextLayer=neurons[i+1];
        for (unsigned k=0; k<hidden.size();++k) {
            hidden[k]->calchiddengrad(nextLayer);
        }
    }

    for (unsigned i=neurons.size()-1;i>0;--i) {
        for (unsigned k=0;k<neurons[i].size()-1;++k) {
            neurons[i][k]->updateInputWeights(neurons[i-1]);
        }
    }
}

void net::getResults(std::vector<double> &Result)  {
    Result.clear();
    for (unsigned i=0; i<neurons.back().size()-1;++i) {
        Result.push_back(neurons.back()[i]->getOutput());
    }
}

void net::saveWeightTemplate(QString filename) {
    std::vector<std::vector<double>> temp_weight;
    std::vector<std::vector<std::vector<double>>> to_save_weight;
    std::vector<std::vector<double>> temp_delta;
    std::vector<std::vector<std::vector<double>>> to_save_delta;
    for (unsigned i=0;i<neurons.size()-1;++i) {
        for (unsigned k=0;k<neurons[i].size();k++) {
            temp_weight.push_back(neurons[i][k]->getOutputWeight().first);
            temp_delta.push_back(neurons[i][k]->getOutputWeight().second);
        }
        to_save_weight.push_back(temp_weight);
        temp_weight.clear();
        to_save_delta.push_back(temp_delta);
        temp_delta.clear();
    }
    QFile alma(filename);
    if (!alma.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
    QString str1="";
    for (unsigned layer=0;layer<to_save_weight.size();++layer) {
        for (unsigned neuron_index=0;neuron_index<to_save_weight[layer].size();++neuron_index) {
            for (unsigned connection_index=0;connection_index<to_save_weight[layer][neuron_index].size();++connection_index) {
                str1+=QString::number(to_save_weight[layer][neuron_index][connection_index])+","+QString::number(to_save_delta[layer][neuron_index][connection_index]);
                if (connection_index!=to_save_weight[layer][neuron_index].size()-1) {
                    str1+=";";
                }
            }
            str1+='\n';
        }
        str1+='\n';
    }
    QTextStream out(&alma);
    out<<str1;
}

net *net::loadWeightTemplate(QString filename) {
    QFile alma(filename);
    if (!alma.open(QIODevice::ReadOnly | QIODevice::Text))
            return nullptr;
    QString str1="";
    QTextStream in(&alma);
    bool prev_was_emptyline=false;
    std::vector<std::vector<std::vector<std::pair<double,double>>>> loaded_data;
    std::vector<std::vector<std::pair<double,double>>> temp_outer;
    std::vector<std::pair<double,double>> temp_inner;
    while (!in.atEnd()) {
        str1=in.readLine();
        if (!prev_was_emptyline && str1=="") {
            prev_was_emptyline=true;
        } else if (!prev_was_emptyline) {
            for (int i=0; i<str1.count(";")+1;++i) {
                temp_inner.push_back({str1.section(";",i,i).section(",",0,0).toDouble(),str1.section(";",i,i).section(",",1,1).toDouble()});
            }
            temp_outer.push_back(temp_inner);
            temp_inner.clear();
        }
        if (prev_was_emptyline) {
            loaded_data.push_back(temp_outer);
            temp_outer.clear();
            prev_was_emptyline=false;
        }
    }
    std::vector<unsigned> topology;
    for (unsigned layer=0;layer<loaded_data.size();++layer) {
        topology.push_back(loaded_data[layer].size()-1);
    }
    topology.push_back(loaded_data.back().back().size()-1);
    return new net(topology,&loaded_data);
}

void net::clear() {
    for (auto it:neurons) {
        for (auto it2:it) {
            delete it2;
        }
    }
    delete this;
}
