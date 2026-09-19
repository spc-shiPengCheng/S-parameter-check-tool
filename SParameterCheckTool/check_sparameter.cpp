#include "check_sparameter.h"

#include <QDebug>

#include <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include <QDebug>

bool isReciprocal(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold)
{
    if(sparameterDataRI.empty())
    {
        qDebug()<< "S parameter is empty.";
        return false;
    }

    size_t portNum = sparameterDataRI[0][0].size();
    size_t frequencyNum = sparameterDataRI.size();


    for(int k=0;k<frequencyNum;k++)
    {
        for(int i=0;i<portNum;i++)
        {
            for(int j=i;j<portNum;j++)
            {
                if(abs(sparameterDataRI[k][i][j] - sparameterDataRI[k][j][i]) > threshold)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

bool isPassive(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold)
{
    if(sparameterDataRI.empty())
    {
        qDebug() << QString("S parameter is empty.");
        return false;
    }

    bool isPassive = true;
    size_t portNum = sparameterDataRI[0].size();
    size_t frequencyNum = sparameterDataRI.size();

    for(int freq=0;freq<frequencyNum;freq++)
    {
        Eigen::MatrixXcd SMatrix(portNum,portNum);
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                SMatrix(i,j) = sparameterDataRI[freq][i][j];
            }
        }

        Eigen::JacobiSVD<Eigen::MatrixXcd> svd(SMatrix);
        Eigen::MatrixXd singularValues = svd.singularValues();
        double maxSingular = singularValues.maxCoeff();

        if(maxSingular > 1+threshold)
        {
            isPassive = false;
        }
    }

    return isPassive;
}

bool isCausal(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold)
{
    qDebug("No data.");
    return true;
}

std::vector<double> getSingular(const std::vector<std::vector<std::vector<std::complex<double> > > > &sparameterDataRI)
{
    if(sparameterDataRI.empty())
    {
        qDebug() << QString("S parameter is empty.");
        return std::vector<double>();
    }

    size_t portNum = sparameterDataRI[0].size();
    size_t frequencyNum = sparameterDataRI.size();
    std::vector<double> singularData(frequencyNum);

    for(int freq=0;freq<frequencyNum;freq++)
    {
        Eigen::MatrixXcd SMatrix(portNum,portNum);
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                SMatrix(i,j) = sparameterDataRI[freq][i][j];
            }
        }

        Eigen::JacobiSVD<Eigen::MatrixXcd> svd(SMatrix);
        Eigen::MatrixXd singularValues = svd.singularValues();
        double maxSingular = singularValues.maxCoeff();
        singularData[freq] = maxSingular;
    }

    return singularData;
}
