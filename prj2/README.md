### Polygon Clipping and Viewport-to-Window Mapping

Implemented Sutherland-Hodgeman polygon clipping algorithm, flood fill polygon filling algorithm and window-to-viewport mapping.
Allows users to draw polygons by selecting the vertices through mouse-click.
My polygon clipping implementation will autonatically clip the drawn polygon, which results in only the portions of the polygon within the window being rendered.
Users can select a seed pixel within the drawn polygon and my flood fill implementation will fill the inside of the polygon with the color red.
The image within the window will also be automatically mapped to the viewport by my window-to-viewport mapping implementation.
