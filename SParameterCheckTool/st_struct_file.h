/*
 *Author:spc
 *Description: Define data struct for project.
 */

#pragma once

#include <complex>
#include <QString>


/*
 * S parameter file data content.
 */
struct snp
{
    QString filePath = "";
    QString fileName = "";
    std::vector<double> frequency = std::vector<double>();
    double frequencyUnit = 1;
    QString frequencyUnitLabel = "";
    std::vector<std::vector<std::vector<std::complex<double>>>> sparametrRI;
    std::vector<std::vector<std::vector<std::complex<double>>>> yparametrRI;
    std::vector<std::vector<std::vector<std::complex<double>>>> zparametrRI;
    double referenceZ = 0;
    int portNum = 0;

    void reset()
    {
        filePath = "";
        fileName = "";
        frequency.clear();
        frequencyUnit = 1;
        frequencyUnitLabel = "";
        sparametrRI.clear();
        yparametrRI.clear();
        zparametrRI.clear();
        referenceZ = 0;
        portNum = 0;
    }
};


/*
 * Show parameter tyep for ui.
 */
enum DisplayParameterType
{
    S = 0,
    Y,
    Z,
};
enum DisplayDataType
{
    REAL_IMAG = 0,
    MAG_ANGLE,
    DB_ANGLE,
    SMITH,
};

