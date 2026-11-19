## Future Demand Forecasting

After model comparison and time-series cross-validation, the forecasting pipeline was extended to generate future demand estimates.

The program:

`22_Future_Demand_Forecast.py`

was developed for this purpose.

### Selected Model

Linear Regression was used for the future forecasting demonstration.

During five-fold time-series cross-validation on the synthetic demand dataset, Linear Regression produced:

Mean MAE  : 1.603
Mean RMSE : 1.973
Mean R²   : 0.531


## Finland Market-Informed Machine-Learning Evaluation

### Purpose

After the initial synthetic proof-of-concept experiment, a second and more realistic controlled evaluation environment was created.

The purpose of this phase was to test the demand-forecasting pipeline under a longer time horizon and under conditions that resemble Finnish retail demand patterns more closely.

The product-level demand remained synthetic.

The environment was therefore classified as:

`SEMI_SYNTHETIC_FINLAND`

The resulting metrics must not be interpreted as observed Finnish commercial forecasting performance.

---

## Finland Market-Informed Demand Environment

The program:

`26_Finland_Market_Simulation.py`

generated daily demand data covering:

`2023-01-01` to `2026-09-30`

Total observations:

`1,369`

The controlled demand environment included:

- weekday variation,
- monthly seasonality,
- Finnish holidays and special days,
- Christmas-shopping period,
- Black Friday period,
- planned promotions,
- market-context variation,
- random consumer variation,
- positive and negative demand shocks.

The simulation generated:

Mean daily demand : 13.99 units
Minimum demand    : 4 units
Maximum demand    : 39 units