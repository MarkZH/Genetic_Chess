import numpy as np

picture_file_args = {'dpi': 600,
                     'format': 'png'}

plot_params = {'legend text size': 'x-small',
               'scatter dot size': 1,
               'plot line weight': 1,
               'x-axis weight': 0.5,
               'x-axis color': 'k',
               'bar color': 'k'}


def moving_mean(x: np.array, window: int) -> np.array:
    x_avg = np.convolve(x, np.ones(window)/window, mode="valid")
    return x_avg


def centered_x_axis(x: np.array, y: np.array) -> np.array:
    left_margin = (len(x) - len(y)) // 2
    right_margin = len(x) - len(y) - left_margin
    return x[left_margin : -right_margin or len(x)]
