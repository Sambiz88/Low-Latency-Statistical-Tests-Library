import numpy as np
import pandas as pd
import statsmodels.api as sm
from statsmodels.stats.stattools import durbin_watson

df = pd.DataFrame()
df.read_csv('spy.csv')