/*
 * Author: spc
 * Description: Check S parameter characteristics.
 */

#pragma once


#include <vector>
#include <complex>


bool isReciprocal(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold = 1e-6);

bool isPassive(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold = 1e-6);

std::vector<double> getSingular(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI);

bool isCausal(const std::vector<std::vector<std::vector<std::complex<double>>>>& sparameterDataRI, double threshold = 1e-6);

void s2y(const std::vector<std::vector<std::vector<std::complex<double>>>>& sParameterDataRI,
         std::vector<std::vector<std::vector<std::complex<double>>>>& yParameterDataRI,
         double referenceZ);

void s2z(const std::vector<std::vector<std::vector<std::complex<double>>>>& sParameterDataRI,
         std::vector<std::vector<std::vector<std::complex<double>>>>& zParameterDataRI,
         double referenceZ);

void passicityEnforce(const std::vector<std::vector<std::vector<std::complex<double>>>>& sParameterDataRI,
                      std::vector<std::vector<std::vector<std::complex<double>>>>& sParameterDataRIPassive);

void computeGroupDelay(const std::vector<std::vector<std::vector<std::complex<double>>>>& sParameterDataRI,
                       std::vector<std::vector<std::vector<double>>>& sParameterDataGroupDelay);
