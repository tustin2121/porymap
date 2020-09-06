**********************
Scripting Capabilities
**********************

Porymap is extensible via scripting capabilities. This allows the user to write custom JavaScript (technically, ECMAScript) files to support enhanced workflows, without having to fork Porymap itself. While the possibilities are endless, some useful examples of scripting might be:

- Toggle Day/Night Palettes
- Custom Map Painting Brushes
- Detect Tile Errors
- Show Diagonistic Information
- Procedurally Generated Maps
- Randomize Grass Patterns

Writing a Custom Script
-----------------------

Let's write a custom script that will randomize grass patterns when the user is editing the map. This is useful, since it's cumbersome to manually add randomness to grass patches. With the custom script, it will happen automatically. Whenever the user paints a grass tile onto the map, the script will overwrite the tile with a random grass tile instead.

First, create a new script file called ``my_script.js``--place it in the project directory (e.g. ``pokefirered/``).

Next, open the Porymap project config file, ``porymap.project.cfg``, in the project directory. Add the script file to the ``custom_scripts`` configuration value. Multiple script files can be loaded by separating the filepaths with a comma.

.. code-block::

	custom_scripts=my_script.js

Now that Porymap is configured to load the script file, let's write the actual code that will power the grass-randomizer. Scripts have access to several "callbacks" for events that occur while Porymap is running. This means our script can define functions for each of these callbacks. We're interested in the ``onBlockChanged()`` callback, since we want our script to take action whenever a user paints a block on the map.

.. code-block:: js
	
	// Porymap callback when a block is painted.
	export function onBlockChanged(x, y, prevBlock, newBlock) {
	    // Grass-randomizing logic goes here.
	}

It's very **important** to remember to ``export`` the callback functions in the script. Otherwise, Porymap will not be able to execute them.

In addition to the callbacks, Porymap also supports a scripting API so that the script can interact with Porymap in interesting ways. For example, a script can change a block or add overlay text on the map. Since we want to paint random grass tiles, we'll be using the ``map.setMetatileId()`` function. Let's fill in the rest of the grass-randomizing code.

.. code-block:: js

	function randInt(min, max) {
	    min = Math.ceil(min);
	    max = Math.floor(max);
	    return Math.floor(Math.random() * (max - min)) + min;
	}

	// These are the grass metatiles in pokefirered.
	const grassTiles = [0x8, 0x9, 0x10, 0x11];

	// Porymap callback when a block is painted.
	export function onBlockChanged(x, y, prevBlock, newBlock) {
	    // Check if the user is painting a grass tile.
	    if (grassTiles.indexOf(newBlock.metatileId) != -1) {
	        // Choose a random grass tile and paint it on the map.
	        const i = randInt(0, grassTiles.length);
	        map.setMetatileId(x, y, grassTiles[i]);
	    }
	}

Let's test the script out by re-launching Porymap. If we try to paint grass on the map, we should see our script inserting a nice randomized grass pattern.

.. figure:: images/scripting-capabilities/porymap-scripting-grass.gif
    :alt: Grass-Randomizing Script

    Grass-Randomizing Script

Registering Script Actions
--------------------------

The grass-randomizer script above happens implicitly when the user paints on the map. However, other times we probably want to call the custom script on demand. One of the API functions Porymap provides is the ability to trigger scripting functions from the ``Tools`` menu, or a keyboard shortcut. To do this, we will usually want to register the action when the project loads. Here is an example script where some custom actions are registered.

.. code-block:: js

	function applyNightTint() {
	    // Apply night palette tinting...
	}

	// Porymap callback when project is opened.
	export function onProjectOpened(projectPath) {
        map.registerAction("applyNightTint", "View Night Tint", "T")
	}

Then, to trigger the ``applyNightTint()`` function, we could either click ``Tools -> View Night Tint`` or use the ``T`` keyboard shortcut.

Now that we have an overview of how to utilize Porymap's scripting capabilities, the entire scripting API is documented below.

Scripting API
-------------

Callbacks
~~~~~~~~~

.. js:function:: onProjectOpened(projectPath)

   Called when Porymap successfully opens a project.

   :param string projectPath: the directory path of the opened project

.. js:function:: onProjectClosed(projectPath)

   Called when Porymap closes a project. For example, this is called when opening a different project.

   :param string projectPath: the directory path of the closed project

.. js:function:: onMapOpened(mapName)

   Called when a map is opened.

   :param string mapName: the name of the opened map

.. js:function:: onBlockChanged(x, y, prevBlock, newBlock)

   Called when a block is changed on the map. For example, this is called when a user paints a new tile or changes the collision property of a block.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :param object prevBlock: the block's state before it was modified. The object's shape is ``{metatileId, collision, elevation, rawValue}``
   :param object newBlock: the block's new state after it was modified. The object's shape is ``{metatileId, collision, elevation, rawValue}``

Functions
~~~~~~~~~

All scripting functions are callable via the global ``map`` object.

Map Editing Functions
^^^^^^^^^^^^^^^^^^^^^

The following functions are related to editing the map's blocks or retrieving information about them.

.. js:function:: map.getBlock(x, y)

   Gets a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :returns {metatileId, collision, elevation, rawValue}: the block object

.. js:function:: map.setBlock(x, y, metatileId, collision, elevation, forceRedraw = true, commitChanges = true)

   Sets a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :param number metatileId: the metatile id of the block
   :param number collision: the collision of the block (``0`` = passable, ``1`` = impassable)
   :param number elevation: the elevation of the block
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.getMetatileId(x, y)

   Gets the metatile id of a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :returns number: the metatile id of the block

.. js:function:: map.setMetatileId(x, y, metatileId, forceRedraw = true, commitChanges = true)

   Sets the metatile id of a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :param number metatileId: the metatile id of the block
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.getCollision(x, y)

   Gets the collision of a block in the currently-opened map. (``0`` = passable, ``1`` = impassable)

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :returns number: the collision of the block

.. js:function:: map.setCollision(x, y, collision, forceRedraw = true, commitChanges = true)

   Sets the collision of a block in the currently-opened map. (``0`` = passable, ``1`` = impassable)

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :param number collision: the collision of the block
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.getElevation(x, y)

   Gets the elevation of a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :returns number: the elevation of the block

.. js:function:: map.setElevation(x, y, elevation, forceRedraw = true, commitChanges = true)

   Sets the elevation of a block in the currently-opened map.

   :param number x: x coordinate of the block
   :param number y: y coordinate of the block
   :param number elevation: the elevation of the block
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.setBlocksFromSelection(x, y, forceRedraw = true, commitChanges = true)

   Sets blocks on the map using the user's current metatile selection.

   :param number x: initial x coordinate
   :param number y: initial y coordinate
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.bucketFill(x, y, metatileId, forceRedraw = true, commitChanges = true)

   Performs a bucket fill of a metatile id, starting at the given coordinates.

   :param number x: initial x coordinate
   :param number y: initial y coordinate
   :param number metatileId: metatile id to fill
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.bucketFillFromSelection(x, y, forceRedraw = true, commitChanges = true)

   Performs a bucket fill using the user's current metatile selection, starting at the given coordinates.

   :param number x: initial x coordinate
   :param number y: initial y coordinate
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.magicFill(x, y, metatileId, forceRedraw = true, commitChanges = true)

   Performs a magic fill of a metatile id, starting at the given coordinates.

   :param number x: initial x coordinate
   :param number y: initial y coordinate
   :param number metatileId: metatile id to magic fill
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.magicFillFromSelection(x, y, forceRedraw = true, commitChanges = true)

   Performs a magic fill using the user's current metatile selection, starting at the given coordinates.

   :param number x: initial x coordinate
   :param number y: initial y coordinate
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.shift(xDelta, yDelta, forceRedraw = true, commitChanges = true)

   Performs a shift on the map's blocks.

   :param number xDelta: number of blocks to shift horizontally
   :param number yDelta: number of blocks to shift vertically
   :param boolean forceRedraw: Force the map view to refresh. Defaults to ``true``. Redrawing the map view is expensive, so set to ``false`` when making many consecutive map edits, and then redraw the map once using ``map.redraw()``.
   :param boolean commitChanges: Commit the changes to the map's edit/undo history. Defaults to ``true``. When making many related map edits, it can be useful to set this to ``false``, and then commit all of them together with ``map.commit()``.

.. js:function:: map.getDimensions()

   Gets the dimensions of the currently-opened map.

   :returns {width, height}: the dimensions of the map

.. js:function:: map.getWidth()

   Gets the width of the currently-opened map.

   :returns number: the width of the map

.. js:function:: map.getHeight()

   Gets the height of the currently-opened map.

   :returns number: the height of the map

.. js:function:: map.setDimensions(width, height)

   Sets the dimensions of the currently-opened map.

   :param number width: width in blocks
   :param number height: height in blocks

.. js:function:: map.setWidth(width)

   Sets the width of the currently-opened map.

   :param number width: width in blocks

.. js:function:: map.setHeight()

   Sets the height of the currently-opened map.

   :param number height: height in blocks

.. js:function:: map.redraw()

   Redraws the entire map area. Useful when delaying map redraws using ``forceRedraw = false`` in certain map editing functions.

.. js:function:: map.commit()

   Commits any uncommitted changes to the map's edit/undo history. Useful when delaying commits using ``commitChanges = false`` in certain map editing functions.

Map Overlay Functions
^^^^^^^^^^^^^^^^^^^^^

The following functions are related to an overlay that is drawn on top of the map area. Text, images, and shapes can be drawn using these functions.

.. js:function:: map.clearOverlay()

   Clears and erases all overlay items that were previously-added to the map.

.. js:function:: map.addText(text, x, y, color = "#000000", size = 12)

   Adds a text item to the overlay.

   :param string text: the text to display
   :param number x: the x pixel coordinate of the text
   :param number y: the y pixel coordinate of the text
   :param string color: the color of the text. Can be specified as "#RRGGBB" or "#AARRGGBB". Defaults to black.
   :param number size: the font size of the text. Defaults to 12.

.. js:function:: map.addRect(x, y, width, height, color = "#000000")

   Adds a rectangle outline item to the overlay.

   :param number x: the x pixel coordinate of the rectangle's top-left corner
   :param number y: the y pixel coordinate of the rectangle's top-left corner
   :param number width: the pixel width of the rectangle
   :param number height: the pixel height of the rectangle
   :param string color: the color of the rectangle. Can be specified as "#RRGGBB" or "#AARRGGBB". Defaults to black.

.. js:function:: map.addFilledRect(x, y, width, height, color = "#000000")

   Adds a filled rectangle item to the overlay.

   :param number x: the x pixel coordinate of the rectangle's top-left corner
   :param number y: the y pixel coordinate of the rectangle's top-left corner
   :param number width: the pixel width of the rectangle
   :param number height: the pixel height of the rectangle
   :param string color: the color of the rectangle. Can be specified as "#RRGGBB" or "#AARRGGBB". Defaults to black.

.. js:function:: map.addImage(x, y, filepath)

   Adds an image item to the overlay.

   :param number x: the x pixel coordinate of the image's top-left corner
   :param number y: the y pixel coordinate of the image's top-left corner
   :param string filepath: the image's filepath

Tileset Functions
^^^^^^^^^^^^^^^^^

The following functions are related to tilesets and how they are rendered. The functions with "preview" in their name operate on a "fake" version of the palette colors. This means that changing these "preview" colors won't affect the actual tileset colors in the project. A good use of the "preview" palettes would be Day/Night tints, for example.

.. js:function:: map.getPrimaryTilesetPalettePreview(paletteIndex)

   Gets a palette from the primary tileset of the currently-opened map.

   :param number paletteIndex: the palette index
   :returns array: array of colors. Each color is a 3-element RGB array

.. js:function:: map.setPrimaryTilesetPalettePreview(paletteIndex, colors)

   Sets a palette in the primary tileset of the currently-opened map. This will NOT affect the true underlying colors--it only displays these colors in the map-editing area of Porymap.

   :param number paletteIndex: the palette index
   :param array colors: array of colors. Each color is a 3-element RGB array

.. js:function:: map.getPrimaryTilesetPalettesPreview()

   Gets all of the palettes from the primary tileset of the currently-opened map.

   :returns array: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.setPrimaryTilesetPalettesPreview(palettes)

   Sets all of the palettes in the primary tileset of the currently-opened map. This will NOT affect the true underlying colors--it only displays these colors in the map-editing area of Porymap.

   :param array palettes: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.getSecondaryTilesetPalettePreview(paletteIndex)

   Gets a palette from the secondary tileset of the currently-opened map.

   :param number paletteIndex: the palette index
   :returns array: array of colors. Each color is a 3-element RGB array

.. js:function:: map.setSecondaryTilesetPalettePreview(paletteIndex, colors)

   Sets a palette in the secondary tileset of the currently-opened map. This will NOT affect the true underlying colors--it only displays these colors in the map-editing area of Porymap.

   :param number paletteIndex: the palette index
   :param array colors: array of colors. Each color is a 3-element RGB array

.. js:function:: map.getSecondaryTilesetPalettesPreview()

   Gets all of the palettes from the secondary tileset of the currently-opened map.

   :returns array: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.setSecondaryTilesetPalettesPreview(palettes)

   Sets all of the palettes in the secondary tileset of the currently-opened map. This will NOT affect the true underlying colors--it only displays these colors in the map-editing area of Porymap.

   :param array palettes: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.getPrimaryTilesetPalette(paletteIndex)

   Gets a palette from the primary tileset of the currently-opened map.

   :param number paletteIndex: the palette index
   :returns array: array of colors. Each color is a 3-element RGB array

.. js:function:: map.setPrimaryTilesetPalette(paletteIndex, colors)

   Sets a palette in the primary tileset of the currently-opened map. This will permanently affect the palette and save the palette to disk.

   :param number paletteIndex: the palette index
   :param array colors: array of colors. Each color is a 3-element RGB array

.. js:function:: map.getPrimaryTilesetPalettes()

   Gets all of the palettes from the primary tileset of the currently-opened map.

   :returns array: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.setPrimaryTilesetPalettes(palettes)

   Sets all of the palettes in the primary tileset of the currently-opened map. This will permanently affect the palettes and save the palettes to disk.

   :param array palettes: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.getSecondaryTilesetPalette(paletteIndex)

   Gets a palette from the secondary tileset of the currently-opened map.

   :param number paletteIndex: the palette index
   :returns array: array of colors. Each color is a 3-element RGB array

.. js:function:: map.setSecondaryTilesetPalette(paletteIndex, colors)

   Sets a palette in the secondary tileset of the currently-opened map. This will permanently affect the palette and save the palette to disk.

   :param number paletteIndex: the palette index
   :param array colors: array of colors. Each color is a 3-element RGB array

.. js:function:: map.getSecondaryTilesetPalettes()

   Gets all of the palettes from the secondary tileset of the currently-opened map.

   :returns array: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.setSecondaryTilesetPalettes(palettes)

   Sets all of the palettes in the secondary tileset of the currently-opened map. This will permanently affect the palettes and save the palettes to disk.

   :param array palettes: array of arrays of colors. Each color is a 3-element RGB array

.. js:function:: map.getPrimaryTileset()

   Gets the name of the primary tileset for the currently-opened map.

   :returns string: primary tileset name

.. js:function:: map.setPrimaryTileset(tileset)

   Sets the primary tileset for the currently-opened map.

   :param string tileset: the tileset name

.. js:function:: map.getSecondaryTileset()

   Gets the name of the secondary tileset for the currently-opened map.

   :returns string: secondary tileset name

.. js:function:: map.setSecondaryTileset(tileset)

   Sets the secondary tileset for the currently-opened map.

   :param string tileset: the tileset name

.. js:function:: map.getMetatileLayerOrder()

   Gets the order that metatile layers are rendered.

   :return array: array of layers. The bottom layer is represented as 0.

.. js:function:: map.setMetatileLayerOrder(order)

   Sets the order that metatile layers are rendered.

   :param array: array of layers. The bottom layer is represented as 0.

.. js:function:: map.getMetatileLayerOpacity()

   Gets the opacities that metatile layers are rendered with.

   :return array: array of opacities for each layer. The bottom layer is the first element.

.. js:function:: map.setMetatileLayerOpacity(opacities)

   Sets the opacities that metatile layers are rendered with.

   :param array: array of opacities for each layer. The bottom layer is the first element.


Settings Functions
^^^^^^^^^^^^^^^^^^

The following functions are related to settings.

.. js:function:: map.getGridVisibility()

   Gets the visibility of the map grid overlay.

   :returns boolean: grid visibility

.. js:function:: map.setGridVisibility(visible)

   Sets the visibility of the map grid overlay.

   :param boolean visible: grid visibility

.. js:function:: map.getBorderVisibility()

   Gets the visibility of the map's border.

   :returns boolean: border visibility

.. js:function:: map.setBorderVisibility(visible)

   Sets the visibility of the map's border.

   :param boolean visible: border visibility

.. js:function:: map.getSmartPathsEnabled()

   Gets the toggle state of smart paths.

   :returns boolean: smart paths enabled

.. js:function:: map.setSmartPathsEnabled(enabled)

   Sets the toggle state of smart paths.

   :param boolean enabled: smart paths enabled

Utility Functions
^^^^^^^^^^^^^^^^^

These are some miscellaneous functions that can be very useful when building custom scripts.

.. js:function:: map.registerAction(functionName, actionName, shortcut = "")

   Registers a JavaScript function to an action that can be manually triggered in Porymap's ``Tools`` menu. Optionally, a keyboard shortcut (e.g. ``"Ctrl+P"``) can also be specified, assuming it doesn't collide with any existing shortcuts used by Porymap.

   :param string functionName: name of the JavaScript function
   :param string actionName: name of the action that will be displayed in the ``Tools`` menu
   :param string shortcut: optional keyboard shortcut

.. js:function:: map.setTimeout(func, delayMs)

   This behaves essentially the same as JavaScript's ``setTimeout()`` that is used in web browsers or NodeJS. The ``func`` argument is a JavaScript function (NOT the name of a function) which will be executed after a delay. This is useful for creating animations or refreshing the overlay at constant intervals.

   :param function func: a JavaScript function that will be executed later
   :param number delayMs: the number of milliseconds to wait before executing ``func``

.. js:function:: map.log(message)

   Logs a message to the Porymap log file. This is useful for debugging custom scripts.

   :param string message: the message to log
