import sys
#from PyQt5 import QtCore
from PyQt5.QtCore import Qt, pyqtSignal, QTimer
from PyQt5.QtWidgets import QApplication, QMainWindow, QWidget, QHBoxLayout
import pyqtgraph as pg
from skimage import io, transform
import numpy as np
from dataclasses import dataclass

@dataclass
class MouseEvent:
    # button: 1 - lclick, 2 - rclick, 3 - ?, 4 - mclick
    # db_click: Whether or not click was a doubleclick
    # source: index of KeyPressWindow that the click was in
    button: int
    x: int
    y: int
    db_click: bool = None
    source: int = -1

pg.setConfigOption('background', 'k')
pg.setConfigOption('foreground', 'w')

class ImagePlot(pg.GraphicsLayoutWidget):
    sigKeyPress = pyqtSignal(object)
    sigMouseClicked = pyqtSignal(object)
    color_dict = {'r':(255,0,0), 'g':(0,255,0),'b':(0,0,255),'p':(255,0,255),'y':(255,255,0)}

    def __init__(self, use_roi=False, movable_roi=True):
        self.pti = 0

        super(ImagePlot, self).__init__()

        self.p1 = pg.PlotItem() 
        self.addItem(self.p1)
        self.p1.vb.invertY(True) # Images need inverted Y axis
        self.p1.showAxes(False)

        # Use ScatterPlotItem to draw points
        self.scatterItem = pg.ScatterPlotItem(
            size=10, 
            pen=pg.mkPen(None), 
            brush=pg.mkBrush(255, 0, 0),
            hoverable=True,
            hoverBrush=pg.mkBrush(0, 255, 255)
        )
        self.scatterItem.setZValue(2) # Ensure scatterPlotItem is always at top
        self.points = np.zeros((5,2))

        self.p1.addItem(self.scatterItem)

        self.use_roi = use_roi
        if self.use_roi:
            self.roi = pg.RectROI(pos=(0,0), size=(100,100), movable=movable_roi, aspectLocked=True)
            self.roi.setPen((255,0,0))
            self.roi.setZValue(20)
            self.p1.addItem(self.roi)


    def setImage(self, image, size=None):
        self.p1.clear()
        self.p1.addItem(self.scatterItem)
        if self.use_roi:
            self.p1.addItem(self.roi)

        # pg.ImageItem.__init__ method takes input as an image array
        if isinstance(image, str):
            image = io.imread(image_path)

        if size is not None:
            image = transform.resize(image, size)

        self.image_item = pg.ImageItem(image)
        self.image_item.setOpts(axisOrder='row-major')
        self.p1.getViewBox().setAspectLocked(True, ratio=(image.shape[1]/image.shape[0]))
        self.p1.addItem(self.image_item)


    def keyPressEvent(self, event):
        if event.text().isdigit() and int(event.text()) <= 5 and int(event.text()) > 0:
            self.pti = int(event.text()) - 1
        self.sigKeyPress.emit(event)


    def mousePressEvent(self, event):
        point = self.p1.vb.mapSceneToView(event.pos()) # get the point clicked
        # Get pixel position of the mouse click
        x, y = int(point.x()), int(point.y())
        if event.button() == 4:
            self.points[self.pti, :] = [x, y]
            self.setPoints()
        super().mousePressEvent(event)
        ev = MouseEvent(event.button(), x, y)
        self.sigMouseClicked.emit(event)

    def setPoints(self):
        spots = []
        ind = 0
        for point in self.points:
            if point.all() != 0:
                spot = {'pos':point, 'brush':self.color_dict[['r','g','b','p','y'][ind]], \
                        'symbol':'+'}
                spots.append(spot)
            ind += 1
        self.scatterItem.setData(spots=spots)
        

if __name__ == "__main__":

    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    app = QApplication([])
    win = QMainWindow()

    central_win = QWidget()
    layout = QHBoxLayout()
    central_win.setLayout(layout)
    win.setCentralWidget(central_win)

    image_plot1 = ImagePlot()
    image_plot2 = ImagePlot()
    layout.addWidget(image_plot1)
    layout.addWidget(image_plot2)

    image_plot1.setImage('roitest.png', )
    image_plot2.setImage('roitest.png')
    # You can access points by accessing image_plot1.points
    win.show()

    if (sys.flags.interactive != 1) or not hasattr(Qt.QtCore, "PYQT_VERSION"):
        QApplication.instance().exec_()
