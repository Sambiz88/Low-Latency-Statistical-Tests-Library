from scipy import stats
import pandas as pd

df = pd.read_csv('spy.csv')
data = df["return"].dropna()
jb_stat, p_value = stats.jarque_bera(data)

print(f"Jarque-Bera Statistic: {jb_stat:.4f}")
print(f"P-value: {p_value:.4f}")

if p_value < 0.05:
    print("Reject the null hypothesis: the data is not normally distributed")
else:
    print("Fail to reject the null hypothesis: the data is normally distributed")