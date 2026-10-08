/*
 * Author: spc
 * Description: Check S parameter characteristics.
 */
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

void s2y(const std::vector<std::vector<std::vector<std::complex<double> > > > &sParameterDataRI,
         std::vector<std::vector<std::vector<std::complex<double> > > > &yParameterDataRI,
         double referenceZ)
{
    if(sParameterDataRI.empty())
    {
        qDebug() << QString("S parameter is empty.");
        return ;
    }

    size_t portNum = sParameterDataRI[0].size();
    size_t frequencyNum = sParameterDataRI.size();
    yParameterDataRI.clear();

    Eigen::MatrixXcd I = Eigen::MatrixXcd::Identity(portNum,portNum);
    for(int freq=0;freq<frequencyNum;freq++)
    {
        Eigen::MatrixXcd SMatrix(portNum,portNum);
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                SMatrix(i,j) = sParameterDataRI[freq][i][j];
            }
        }

        Eigen::MatrixXcd YMatrix = 1/referenceZ * (I + SMatrix).inverse() * (I - SMatrix);

        std::vector<std::vector<std::complex<double>>> yParameter(portNum,
                                                                  std::vector<std::complex<double>>(portNum,0));
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                yParameter[i][j] = YMatrix(i,j);
            }
        }
        yParameterDataRI.push_back(yParameter);
    }

}

void s2z(const std::vector<std::vector<std::vector<std::complex<double> > > > &sParameterDataRI,
         std::vector<std::vector<std::vector<std::complex<double> > > > &zParameterDataRI,
         double referenceZ)
{
    if(sParameterDataRI.empty())
    {
        qDebug() << QString("S parameter is empty.");
        return ;
    }

    size_t portNum = sParameterDataRI[0].size();
    size_t frequencyNum = sParameterDataRI.size();
    zParameterDataRI.clear();

    Eigen::MatrixXcd I = Eigen::MatrixXcd::Identity(portNum,portNum);
    for(int freq=0;freq<frequencyNum;freq++)
    {
        Eigen::MatrixXcd SMatrix(portNum,portNum);
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                SMatrix(i,j) = sParameterDataRI[freq][i][j];
            }
        }

        Eigen::MatrixXcd ZMatrix = referenceZ * (I - SMatrix).inverse() * (I + SMatrix);

        std::vector<std::vector<std::complex<double>>> zParameter(portNum,
                                                                  std::vector<std::complex<double>>(portNum,0));
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                zParameter[i][j] = ZMatrix(i,j);
            }
        }
        zParameterDataRI.push_back(zParameter);
    }
}

void passicityEnforce(const std::vector<std::vector<std::vector<std::complex<double> > > > &sParameterDataRI,
                      std::vector<std::vector<std::vector<std::complex<double> > > > &sParameterDataRIPassive)
{
    if(sParameterDataRI.empty())
    {
        qDebug() << QString("S parameter is empty.");
        return ;
    }

    size_t portNum = sParameterDataRI[0].size();
    size_t frequencyNum = sParameterDataRI.size();
    sParameterDataRIPassive.clear();
    sParameterDataRIPassive = sParameterDataRI;

    for(int freq=0;freq<frequencyNum;freq++)
    {
        Eigen::MatrixXcd SMatrix(portNum,portNum);
        for(int i=0;i<portNum;i++)
        {
            for(int j=0;j<portNum;j++)
            {
                SMatrix(i,j) = sParameterDataRI[freq][i][j];
            }
        }

        Eigen::JacobiSVD<Eigen::MatrixXcd> svd(SMatrix, Eigen::ComputeThinU | Eigen::ComputeThinV);
        Eigen::VectorXd  singularValuesVec = svd.singularValues();
        Eigen::MatrixXd singularValues = singularValuesVec.asDiagonal();   // 2x2 对角矩阵
        double maxSingular = singularValues.maxCoeff();
        if(maxSingular > 1)
        {
            for(int i=0;i<portNum;i++)
            {
                for(int j=0;j<portNum;j++)
                {
                    if(singularValues(i,j) > 1)
                    {
                        singularValues(i,j) = 1;
                    }
                }
            }
            Eigen::MatrixXcd SMatrixPassive = svd.matrixU() * singularValues * svd.matrixV().adjoint();

            std::vector<std::vector<std::complex<double>>> sParameter(portNum,
                                                                      std::vector<std::complex<double>>(portNum,0));
            for(int i=0;i<portNum;i++)
            {
                for(int j=0;j<portNum;j++)
                {
                    sParameter[i][j] = SMatrixPassive(i,j);
                }
            }
            sParameterDataRIPassive[freq] = sParameter;
        }
    }
}
