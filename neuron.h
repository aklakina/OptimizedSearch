#ifndef NEURON_H
#define NEURON_H

#include <vector>
#include <cstdlib>



struct Connection {
    double weight;
    double deltaweight;
};

class Neuron
{
public:
    Neuron(unsigned numborofoutputs);
    void feedForward(const std::vector<Neuron *> prevLayer,unsigned _myIndex);
    void setOutput(double val);
    double getOutput();
    static double transfer(double x);
    static double transferderivative(double x);
    void calcoutputgrad(double target);
    void calchiddengrad(const std::vector<Neuron *> nextLayer);
    void updateInputWeights(std::vector<Neuron *> prevLayer);
    void setOutputNumber(unsigned num);
    std::pair<std::vector<double>,std::vector<double>> getOutputWeight();
    void setOutputWeight(std::vector<std::pair<double,double>> loaded);
    static unsigned map_size;
private:
    static double eta;
    static double alpha;
    double output;
    std::vector<Connection> outputweights;
    static double randomWeight(void);
    double sumdow(const std::vector<Neuron *> nextLayer);
    unsigned myIndex;
    double gradient;
};
#endif // NEURON_H
