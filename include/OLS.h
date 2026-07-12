#include <vector>
#include <array>
#include <cmath>
#include <stdexcept>
#include <algorithm>

class OLS {
public:
    OLS() = delete;

    
    template <typename MatrixT, typename VectorT>
    OLS(const MatrixT& xIn, const VectorT& yIn)
        : Y(yIn.begin(), yIn.end())
    {
        X.reserve(xIn.size());
        for (const auto& row : xIn) {
            X.emplace_back(row.begin(), row.end());
        }

        n = static_cast<int>(Y.size());
        k = X.empty() ? 0 : static_cast<int>(X[0].size());

        if (X.size() != Y.size()) {
            throw std::invalid_argument("X and Y must have the same number of rows");
        }
        for (const auto& row : X) {
            if (static_cast<int>(row.size()) != k) {
                throw std::invalid_argument("All rows of X must have the same number of columns");
            }
        }
        if (n <= k) {
            throw std::invalid_argument("Not enough observations for the number of parameters");
        }

        fit();
    }

    void fit() {
        std::vector<std::vector<float>> XtX(k, std::vector<float>(k, 0.0));
        std::vector<float> XtY(k, 0.0);
        for (size_t a{}; a < k; ++a) {
            for (size_t b{}; b < k; ++b) {
                float sum = 0.0;
                for (int i = 0; i < n; ++i) sum += X[i][a] * X[i][b];
                XtX[a][b] = sum;
            }
            float sumY = 0.0;
            for (size_t i{}; i < n; ++i) sumY += X[i][a] * Y[i];
            XtY[a] = sumY;
        }

        std::vector<std::vector<float>> A = XtX;
        for (size_t i{}; i < k; ++i) A[i].push_back(XtY[i]);

        std::vector<std::vector<float>> I(k, std::vector<float>(k, 0.0));
        for (size_t i{}; i < k; ++i) I[i][i] = 1.0;

        constexpr float eps = 1e-12;
        for (size_t col{}; col < k; ++col) {
            int pivotRow = col;
            for (size_t row{col + 1}; row < k; ++row)
                if (std::abs(A[row][col]) > std::abs(A[pivotRow][col])) pivotRow = row;
            std::swap(A[col], A[pivotRow]);
            std::swap(I[col], I[pivotRow]);

            float pivotVal = A[col][col];
            if (std::abs(pivotVal) < eps) {
                throw std::runtime_error("X'X is singular or near-singular (check for collinearity)");
            }
            for (size_t c{}; c <= k; ++c) A[col][c] /= pivotVal;
            for (size_t c{}; c < k; ++c) I[col][c] /= pivotVal;

            for (size_t row{}; row < k; ++row) {
                if (row == col) continue;
                float factor = A[row][col];
                for (size_t c{}; c <= k; ++c) A[row][c] -= factor * A[col][c];
                for (size_t c{}; c < k; ++c) I[row][c] -= factor * I[col][c];
            }
        }

        coefficients.assign(k, 0.0);
        for (size_t i{}; i < k; ++i) coefficients[i] = A[i][k];
        XtXInv = I;

        residuals.assign(n, 0.0);
        rss = 0.0;
        for (size_t i{}; i < n; ++i) {
            float predicted = 0.0;
            for (size_t j{}; j < k; ++j) predicted += X[i][j] * coefficients[j];
            residuals[i] = Y[i] - predicted;
            rss += residuals[i] * residuals[i];
        }
        sigma2 = rss / (n - k);

        standardErrors.assign(k, 0.0);
        for (size_t j{}; j < k; ++j) {
            standardErrors[j] = std::sqrt(sigma2 * XtXInv[j][j]);
        }
        fitted = true;
    }

    float tStat(int coefIndex) const {
        if (!fitted) throw std::runtime_error("Model not fitted");
        return coefficients.at(coefIndex) / standardErrors.at(coefIndex);
    }

    float getRss(){
        return rss;
    }

    private:

        std::vector<std::vector<float>> X;
        std::vector<float> Y;
        int n;
        int k;

        bool fitted = false;
        std::vector<float> coefficients;
        std::vector<float> residuals;
        std::vector<float> standardErrors;
        std::vector<std::vector<float>> XtXInv;
        float rss = 0.0;
        float sigma2 = 0.0;
};