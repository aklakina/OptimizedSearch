#include "neuron.h"
#include <cmath>
#include <time.h>
#include <QDebug>
Neuron::Neuron(unsigned numborofoutputs)
{
    srand(time(NULL));
    for (unsigned c=0; c<numborofoutputs;++c) {
        outputweights.push_back(Connection());
        outputweights.back().weight=randomWeight();
    }
}

double Neuron::randomWeight(void) {
    return rand()/double(RAND_MAX);
}

void Neuron::setOutput(double val) {
    output=val;
}

double Neuron::getOutput() {
    return output;
}

void Neuron::feedForward(const std::vector<Neuron *> prevLayer, unsigned _myIndex) {
    double sum=0;
    for (unsigned i=0;i<prevLayer.size();++i) {
        sum+=prevLayer[i]->output*prevLayer[i]->outputweights[_myIndex].weight;
    }
    output=transfer(sum);
    myIndex=_myIndex;
}

double Neuron::transfer(double x) {
    return tanh(x);
}

double Neuron::transferderivative(double x) {
    return 1/(cosh(x)*cosh(x));
}

void Neuron::calcoutputgrad(double target) {
    double delta= target-output;
    gradient=delta*transferderivative(output);
}

void Neuron::calchiddengrad(const std::vector<Neuron *> nextLayer) {
    double dow=sumdow(nextLayer);
    gradient=dow*transferderivative(output);
}

double Neuron::sumdow(const std::vector<Neuron *> nextLayer) {
    double sum=0;
    for (unsigned i=0; i<nextLayer.size()-1;++i) {
        sum+=outputweights[i].weight+nextLayer[i]->gradient;
    }
    return sum;
}

double Neuron::eta=0.8;
double Neuron::alpha=0.2;
unsigned Neuron::map_size=0;

void Neuron::updateInputWeights(std::vector<Neuron*> prevLayer) {
    for (unsigned i=0; i<prevLayer.size();++i) {
        double olddeltaWeight=prevLayer[i]->outputweights[myIndex].deltaweight;
        double newdeltaWeight=eta*prevLayer[i]->getOutput()*gradient+alpha*olddeltaWeight;
        prevLayer[i]->outputweights[myIndex].deltaweight=newdeltaWeight;
        prevLayer[i]->outputweights[myIndex].weight+=newdeltaWeight;
    }
}

void Neuron::setOutputNumber(unsigned int num) {
    for (unsigned c=0; c<num;++c) {
        outputweights.push_back(Connection());
        outputweights.back().weight=randomWeight();
    }
}

std::pair<std::vector<double>, std::vector<double> > Neuron::getOutputWeight() {
    std::vector<double> temp;
    std::vector<double> temp2;
    for (unsigned i=0; i<outputweights.size();++i) {
        temp.push_back(outputweights[i].weight);
        temp2.push_back(outputweights[i].deltaweight);
    }
    return {temp,temp2};
}

void Neuron::setOutputWeight(std::vector<std::pair<double, double> > loaded) {
    for (unsigned i=0; i<loaded.size();++i) {
        outputweights[i].weight=loaded[i].first;
        outputweights[i].deltaweight=loaded[i].second;
    }
}
