# Controls:
| Input | Command |
| ----- | ------- | 
| Middle click: | Select point |
| Numbers 1-5:  | Select marker index[^1] |
| `l`           | Load points (reads from `PTS_CSV_READ`) |
| `p`           | Save current point (saved to `PTS_CSV_SAVE`) |
| `c`           | Compile and align images with selected points |
| `o`           | Open a new raw image to edit[^2] |
| `s`           | Save roi of aligned images |
| `m`           | Toggles movablility of roi (the little red square that shows up in the third window) |

[^1]: **NOTE**: you have to lclick on the plot that you want to make this change in each time, its sort of an unfortunate thing, deciding on how to fix it
[^2]: **NOTE**: if you change the raw image, you also will need to set where to save points and where to save the aligned image
# Usability
> Unfortunately, there are 6 variables that you kinda need to set (all the variables that the user needs to set are writted in all caps). 
> `TRACE_PATH` - Path to the traced image
> `TRACE_PATH_SAVE` - Path to the location where you wanna save the aligned traced image
>
> `RAW_PATH` - Path to the location of the raw image (that you're aligning to the tracing)
>
> `RAW_PATH_SAVE` - Where you want to save the aligned raw image
>
> `PTS_CSV_READ` - Where to read the alignment points from
>
> `PTS_CSV_SAVE` - Where to save alignment points
