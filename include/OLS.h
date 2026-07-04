#include <vector>
#include <cmath>
#include <stdexcept>
#include <algorithm>

class OLS {
public:
   
    OLS() = delete;
    OLS(const std::vector<std::vector<double>>& xIn, const std::vector<double>& yIn)
        : X(xIn), Y(yIn), n(yIn.size()), k(xIn.empty() ? 0 : xIn[0].size())
    {
        if (X.size() != Y.size()) {
            throw std::invalid_argument("X and Y must have the same number of rows");
        }
        if (n <= k) {
            throw std::invalid_argument("Not enough observations for the number of parameters");
        }
    }

   
    void fit() {
        // 1. Build XtX and XtY
        std::vector<std::vector<double>> XtX(k, std::vector<double>(k, 0.0));
        std::vector<double> XtY(k, 0.0);

        for (int a = 0; a < k; ++a) {
            for (int b = 0; b < k; ++b) {
                double sum = 0.0;
                for (int i = 0; i < n; ++i) sum += X[i][a] * X[i][b];
                XtX[a][b] = sum;
            }
            double sumY = 0.0;
            for (int i = 0; i < n; ++i) sumY += X[i][a] * Y[i];
            XtY[a] = sumY;
        }

        // 2: Solve (XtX)*b = XtY via Gauss-Jordan

        std::vector<std::vector<double>> A = XtX;
        for (int i = 0; i < k; ++i) A[i].push_back(XtY[i]);   

        std::vector<std::vector<double>> I(k, std::vector<double>(k, 0.0));
        for (int i = 0; i < k; ++i) I[i][i] = 1.0;

        for (int col = 0; col < k; ++col) {
            int pivotRow = col;
            for (int row = col + 1; row < k; ++row)
                if (std::abs(A[row][col]) > std::abs(A[pivotRow][col])) pivotRow = row;
            std::swap(A[col], A[pivotRow]);
            std::swap(I[col], I[pivotRow]);

            double pivotVal = A[col][col];
            for (int c = 0; c <= k; ++c) A[col][c] /= pivotVal;   
            for (int c = 0; c < k; ++c) I[col][c] /= pivotVal;

            for (int row = 0; row < k; ++row) {
                if (row == col) continue;
                double factor = A[row][col];
                for (int c = 0; c <= k; ++c) A[row][c] -= factor * A[col][c];
                for (int c = 0; c < k; ++c) I[row][c] -= factor * I[col][c];
            }
        }

        coefficients.assign(k, 0.0);
        for (int i = 0; i < k; ++i) coefficients[i] = A[i][k];   
        XtXInv = I;                                            

        // 3. Residuals and RSS
        residuals.assign(n, 0.0);
        rss = 0.0;
        for (int i = 0; i < n; ++i) {
            double predicted = 0.0;
            for (int j = 0; j < k; ++j) predicted += X[i][j] * coefficients[j];
            residuals[i] = Y[i] - predicted;
            rss += residuals[i] * residuals[i];
        }
        sigma2 = rss / (n - k);

        // 4. Standard errors
        standardErrors.assign(k, 0.0);
        for (int j = 0; j < k; ++j) {
            standardErrors[j] = std::sqrt(sigma2 * XtXInv[j][j]);
        }

        fitted = true;
    }

    // t-stat convenience
    double tStat(int coefIndex) const {
        return coefficients[coefIndex] / standardErrors[coefIndex];
    }

    // Inputs 
    std::vector<std::vector<double>> X;
    std::vector<double> Y;
    int n;
    int k;

    // Outputs
    bool fitted = false;
    std::vector<double> coefficients;      // alpha, beta, gammas...
    std::vector<double> residuals;         // epsilon_hat
    std::vector<double> standardErrors;    // SE for each coefficient
    std::vector<std::vector<double>> XtXInv;
    double rss = 0.0;
    double sigma2 = 0.0;
};