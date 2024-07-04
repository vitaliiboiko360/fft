#!/usr/bin/python

import plotly.graph_objects as go
import numpy as np
import os

yValues = np.genfromtxt("output.txt", delimiter="\n")
xValues = np.arange(yValues.size)

fig = go.Figure()
# Create and style traces
fig.add_trace(go.Scatter(x=xValues, y=yValues, name='plot',
                         line=dict(color='royalblue', width=2)))
fig.update_layout(yaxis_range=[-5000,5000])
fig.show()

if not os.path.exists("images"):
    os.mkdir("images")

fig.write_image("images/fig1.png")