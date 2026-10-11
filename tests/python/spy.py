import yfinance as yf

df = yf.download("SPY", period="25y")

prices = df["Close"]
spy_returns = prices.pct_change()

spy_returns = spy_returns.rename(columns={"SPY": "return"})
spy_returns.to_csv("spy.csv")