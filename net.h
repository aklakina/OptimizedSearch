#ifndef NET_H
#define NET_H

#include <vector>
#include <QString>
#include "neuron.h"

//typedef std::vector<Neuron> Layer;

class net
{
public:
    net(const std::vector<unsigned> _topology,std::vector<std::vector<std::vector<std::pair<double,double>>>> *loaded_data=nullptr);
    void feedForward(const std::vector<double> &inputVals);
    void backProp(const std::vector<double> &Target);
    void getResults(std::vector<double> &Result);
    void saveWeightTemplate(QString filename);
    static net* loadWeightTemplate(QString filename);
    void clear();
private:
    double error;
    //std::vector<double> inputVals;
    //std::vector<unsigned> topology;
    std::vector<std::vector<Neuron *>> neurons;
    double smoothing;
    double recentaverageerror;
};

#endif // NET_H
