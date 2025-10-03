import numpy as np
import matplotlib.pyplot as plt
import common

def plot_survival(pool_file_name: str) -> None:
    ages: dict[int, int] = {}
    oldest_in_generation: list[int] = []
    largest_ids: list[int] = []
    with open(pool_file_name) as pool_file:
        for line in pool_file:
            if not line.startswith("Still Alive:"):
                continue

            surviving_ids = list(map(int, line.split(":", maxsplit=1)[1].split()))
            largest_ids.append(max(surviving_ids))
            ages = {id_number: ages.get(id_number, 0) + 1 for id_number in surviving_ids}
            oldest_in_generation.append(max(ages.values()))

    fig, ax = plt.subplots()
    scale, label = common.x_axis_scaling(largest_ids)
    ax.scatter(np.array(largest_ids)/scale, oldest_in_generation, common.plot_params["scatter dot size"])
    ax.set_xlabel(f"ID{label}")
    ax.set_ylabel(f"Age (games survived)")
    ax.set_title("Age of oldest player in each generation")
    fig.savefig(f"{pool_file_name}_survival.png", **common.picture_file_args)
