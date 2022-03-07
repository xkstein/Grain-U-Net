def sub_plot(ax, im, cmap="gray", title=None):
    ax.imshow(im, cmap=cmap)
    ax.axis("off")
    if title is not None:
        ax.set_title(title)
