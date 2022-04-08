def sub_plot(ax, im, cmap="gray", title=None, title_color='k'):
    '''Little helper for all the plotting that sometimes happens
    '''
    ax.imshow(im, cmap=cmap)
    ax.axis("off")
    if title is not None:
        ax.set_title(title, color=title_color)
