from rich.segment import Segment
from rich.style import Style

from textual.app import App, ComposeResult
from textual.strip import Strip
from textual.widget import Widget
from textual.widgets import Header, Footer, Placeholder
from textual.reactive import reactive
from render import read_xyz, render_mol, rotate, determine_bond
import numpy as np

class CheckerBoard(Widget):
    """Render an 8x8 checkerboard."""
    update = reactive(np.random.random())
    def render_line(self, y: int) -> Strip:
        """Render a line of the widget. y is relative to the top of the widget."""
        #elemnts, xyz = read_xyz("CH4.xyz")
        #grid = render_mol(xyz, elemnts)
 

        row_index = y   # A checkerboard square consists of 4 rows

        if row_index >= 50:  # Generate blank lines when we reach the end
            return Strip.blank(self.size.width)


        white = Style.parse("on white")  # Get a style object for a white background
        black = Style.parse("on black")  # Get a style object for a black background

        # Generate a list of segments for the line
        segments = [
            Segment(grid[column,row_index],  white)
            for column in range(200)
        ]
        strip = Strip(segments, 200 * 1)
        return strip


class BoardApp(App):
    """A simple app to show our widget."""
    BINDINGS = [("q", "quit","Quit the app"),("w", "up","Move up"),("s", "down","Move down"),("a", "left","Move left"),("d", "right","Move right")]

    def compose(self) -> ComposeResult:
        yield CheckerBoard(id="screen")
        yield Header("Checkerboard")
        yield Footer()
    
    def on_key(self, event):
        ANGLE = 2
        ANGLE = np.deg2rad(ANGLE)
        global grid
        global xyz

        if event.key == "w":
            xyz = rotate(xyz, 0, 0, 1*ANGLE)
           # with open("test.log", "w") as f:
           #     f.write("w") 
            grid = render_mol(xyz, elemnts,bond)
        elif event.key == "s":

            xyz = rotate(xyz, 0, 0, -1*ANGLE)
            grid = render_mol(xyz, elemnts,bond)
        elif event.key == "a":
            xyz = rotate(xyz, -1*ANGLE, 0, 0)
            grid = render_mol(xyz, elemnts,bond)
        elif event.key == "d":
            xyz = rotate(xyz, 1*ANGLE, 0, 0)
            grid = render_mol(xyz, elemnts,bond)
        
        self.query_one(CheckerBoard).update = np.random.random()

     


if __name__ == "__main__":
    global grid
    global elemnts
    global xyz
    global bond
    elemnts, xyz = read_xyz("CH4.xyz")
    bond = determine_bond(xyz, elemnts)
    grid = render_mol(xyz, elemnts,bond)
    app = BoardApp()
    app.run()
    


