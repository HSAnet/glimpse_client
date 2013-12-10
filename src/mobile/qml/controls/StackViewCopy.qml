import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Private 1.0

Item {
    id: root

    /*! \qmlproperty int StackView::depth
        \readonly
        The number of items currently pushed onto the stack.
    */
    readonly property alias depth: root.__depth

    /*! \qmlproperty Item StackView::currentItem
        \readonly
        The currently top-most item in the stack.
    */
    readonly property alias currentItem: root.__currentItem

    /*! The first \l item that should be shown when the StackView is created.
        \a initialItem can take same value as the first argument to \l{StackView::push()}
        {StackView.push()}. Note that this is just a convenience for writing
        \c{Component.onCompleted: stackView.push(myInitialItem)}

        Examples:

        \list
        \li initialItem: Qt.resolvedUrl("MyItem.qml")
        \li initialItem: myItem
        \li initialItem: {"item" : Qt.resolvedUrl("MyRectangle.qml"), "properties" : {"color" : "red"}}
        \endlist
        \sa push
    */
    property var initialItem: null

    /*! \readonly
        \a busy is \c true if a transition is running, and \c false otherwise. */
    readonly property bool busy: __currentTransition !== null

    /*! The transitions to use when pushing or popping items.
        For better understanding on how to apply custom transitions, read \l{Transitions}.
        \sa {Stack::transitions}{Stack.transitions} */
    property StackViewDelegate delegate: StackViewSlideDelegate {}

    /*! Pushes an item onto the stack. The function takes a property list as argument, which
        should contain one or more of the following properties:
        \list
        \li \c item: this property is required, and holds the item you want to push.
        \li \c properties: a list of QML properties that should be assigned
            to the item upon push. These properties will be copied into the item when it is
            loaded (in case of a component or URL), or when it becomes the current item for the
            first time (normally upon push).
        \li \c immediate: set this property to \c true to skip transition effects. When pushing
            an array, you only need to set this property on the first element to make the
            whole operation immediate.
        \li \c replace: set this property to replace the current item on the stack. When pushing
            an array, you only need to set this property on the first element to replace
            as many elements on the stack as inside the array.
        \li \c destroyOnPop: set this property to specify if the item needs to be destroyed
            when its popped off the stack. By default (if \a destroyOnPop is not specified),
            StackView will destroy items pushed as components or URLs. Items
            not destroyed will be re-parented to the original parents they had before being
            pushed onto the stack, and hidden. If you need to set this property, do it with
            care, so that items are not leaked.
        \endlist

        You can also push an array of items (property lists) if you need to push several items
        in one go. A transition will then only occur between the current item and the last
        item in the list. Loading the other items will be deferred until needed.

        Examples:
        \list
        \li stackView.push({item:anItem})
        \li stackView.push({item:aURL, immediate: true, replace: true})
        \li stackView.push({item:aRectangle, properties:{color:"red"}})
        \li stackView.push({item:aComponent, properties:{color:"red"}})
        \li stackView.push({item:aComponent.createObject(), destroyOnPop:true})
        \li stackView.push([{item:anitem, immediate:true}, {item:aURL}])
        \endlist

        \note Note: if the only argument needed is "item", you can apply the following short-
        hand notation: \c{stackView.push(anItem)}.

        Returns the item that became current.

        \sa initialItem
        \sa {Pushing items}
    */
    function push(item) {
        // Note: we support two different APIs in this function; The old meego API, and
        // the new "property list" API. Hence the reason for hiding the fact that you
        // can pass more arguments than shown in the signature:
        if (__recursionGuard(true))
            return
        var properties = arguments[1]
        var immediate = arguments[2]
        var replace = arguments[3]
        var arrayPushed = (item instanceof Array)
        var firstItem = arrayPushed ? item[0] : item
        immediate = (immediate || JSArray.stackView.length === 0)

        if (firstItem && firstItem.item && firstItem.hasOwnProperty("x") === false) {
            // Property list API used:
            immediate = immediate || firstItem.immediate
            replace = replace || firstItem.replace
        }

        // Create, and push, a new javascript object, called "element", onto the stack.
        // This element contains all the information necessary to construct the item, and
        // will, after loaded, also contain the loaded item:
        if (arrayPushed) {
            if (item.length === 0)
                return
            var outElement = replace ? JSArray.pop() : JSArray.current()
            for (var i=0; i<item.length; ++i)
                JSArray.push({itemComponent:item[i], loaded: false, index: __depth, properties: properties});
        } else {
            outElement = replace ? JSArray.pop() : JSArray.current()
            JSArray.push({itemComponent:item, loaded: false, index: __depth, properties: properties})
        }

        var currentElement = JSArray.current()
        var transition = {
            inElement: currentElement,
            outElement: outElement,
            immediate: immediate,
            replace: replace,
            push: true
        }
        __performTransition(transition)
        __recursionGuard(false)
        return __currentItem
    }

    function insert(index, item) {
        if (__recursionGuard(true))
            return

        JSArray.stackView.splice(0, 0, {itemComponent:item, loaded: false, index: index, properties: item["properties"]})
        __depth++

        for(var i=index; i < __depth; ++i)
            JSArray.stackView[i].index = i

        __recursionGuard(false)
    }

    /*! Pops one or more items off the stack. The function takes a property list as argument
        which can contain one or more of the following properties:
        \list
        \li \c item: if specified, all items down to (but not including) \a item will be
            popped off. If \a item is \c null, all items down to (but not including) the
            first item will be popped. If not specified, only the current item will be
            popped.
        \li \c immediate: set this property to \c true to skip transition effects.
        \endlist

        Examples:
        \list
        \li stackView.pop()
        \li stackView.pop({item:someItem, immediate: true})
        \li stackView.pop({immediate: true})
        \li stackView.pop(null)
        \endlist

        \note Note: If the only argument needed is "item", you can apply the following short-
        hand notation: \c{stackView.pop(anItem)}.

        Returns the item that was popped off
        \sa clear()
    */
    function pop(item) {
        if (__depth <= 1)
            return null
        if (item && item.hasOwnProperty("x") === false) {
            // Property list API used:
            var immediate = (item.immediate === true)
            item = item.item
        } else {
            immediate = (arguments[1] === true)
        }

        if (item === __currentItem)
            return

        if (__recursionGuard(true))
            return

        var outElement = JSArray.pop()
        var inElement = JSArray.current()

        if (__depth > 1 && item !== undefined && item !== inElement.item) {
            // Pop from the top until we find 'item', and return the corresponding
            // element. Skip all non-loaded items (except the first), since no one
            // has any references to such items anyway:
            while (__depth > 1 && !JSArray.current().loaded)
                JSArray.pop()
            inElement = JSArray.current()
            while (__depth > 1 && item !== inElement.item) {
                JSArray.pop()
                __cleanup(inElement)
                while (__depth > 1 && !JSArray.current().loaded)
                    JSArray.pop()
                inElement = JSArray.current()
            }
        }

        var transition = {
            inElement: inElement,
            outElement: outElement,
            immediate: immediate,
            replace: false,
            push: false
        }
        __performTransition(transition)
        __recursionGuard(false)
        return outElement.item;
    }

    /*! Remove all items from the stack. No animations will be applied. */
    function clear() {
        if (__recursionGuard(true))
            return
        if (__currentTransition)
            __currentTransition.animation.complete()
        __currentItem = null
        var count = __depth
        for (var i=0; i<count; ++i) {
            var element = JSArray.pop()
            if (element.item)
                __cleanup(element);
        }
        __recursionGuard(false)
    }

    /*! Search for a specific item inside the stack. \a func will
        be called for each item in the stack (with the item as argument)
        until the function returns true. Return value will be the item found. For
        example:
        find(function(item, index) { return item.isTheOne })
        Set \a onlySearchLoadedItems to \c true to not load items that are
        not loaded into memory */
    function find(func, onlySearchLoadedItems) {
        for (var i=__depth-1; i>=0; --i) {
            var element = JSArray.stackView[i];
            if (onlySearchLoadedItems !== true)
                __loadElement(element)
            else if (!element.item)
                continue
            if (func(element.item))
                return element.item
        }
        return null;
    }

    /*! Returns the item at position \a index in
        the stack. If \a dontLoad is true, the
        item will not be forced to load (and \c null
        will be returned if not yet loaded) */
    function get(index, dontLoad)
    {
        if (index < 0 || index >= JSArray.stackView.length)
            return null
        var element = JSArray.stackView[index]
        if (dontLoad !== true) {
            __loadElement(element)
            return element.item
        } else if (element.item) {
            return element.item
        } else {
            return null
        }
    }

    /*! Immediately completes any ongoing transition.
        /sa Animation.complete
      */
    function completeTransition()
    {
        if (__recursionGuard(true))
            return
        if (__currentTransition)
            __currentTransition.animation.complete()
        __recursionGuard(false)
    }

    /********* DEPRECATED API *********/

    /*! \internal
        \deprecated Use Push() instead */
    function replace(item, properties, immediate) {
        push(item, properties, immediate, true)
    }

    /********* PRIVATE API *********/

    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    /*! \internal The currently top-most item on the stack. */
    property Item __currentItem: null
    /*! \internal The number of items currently pushed onto the stack. */
    property int __depth: 0
    /*! \internal Stores the transition info while a transition is ongoing */
    property var __currentTransition: null
    /*! \internal Stops the user from pushing items while preparing a transition */
    property bool __guard: false

    Component.onCompleted: {
        if (initialItem)
            push(initialItem)
    }

    Component.onDestruction: {
        if (__currentTransition)
            __currentTransition.animation.complete()
        __currentItem = null
    }

    /*! \internal */
    function __recursionGuard(use)
    {
        if (use && __guard) {
            console.warn("Warning: StackView: You cannot push/pop recursively!")
            console.trace()
            return true
        }
        __guard = use
    }

    /*! \internal */
    function __loadElement(element)
    {
        if (element.loaded) {
            if (!element.item) {
                element.item = invalidItemReplacement.createObject(root)
                element.item.text = "\nError: The item has been deleted outside StackView!"
            }
            return
        }
        if (!element.itemComponent) {
            element.item = invalidItemReplacement.createObject(root)
            element.item.text = "\nError: Invalid item (item was 'null'). "
                    + "This might indicate that the item was deleted outside StackView!"
            return
        }

        var comp = __resolveComponent(element.itemComponent, element)

        // Assign properties to item:
        if (!element.properties)
            element.properties = {}

        if (comp.hasOwnProperty("createObject")) {
            if (comp.status === Component.Error) {
                element.item = invalidItemReplacement.createObject(root)
                element.item.text = "\nError: Could not load: " + comp.errorString()
            } else {
                element.item = comp.createObject(root, element.properties)
                // Destroy items we create unless the user specified something else:
                if (!element.hasOwnProperty("destroyOnPop"))
                    element.destroyOnPop = true
            }
        } else {
            // comp is already an Item, so just re-parent it into the StackView:
            element.item = comp
            element.originalParent = parent
            element.item.parent = root
            for (var prop in element.properties) {
                if (element.item.hasOwnProperty(prop))
                    element.item[prop] = element.properties[prop];
            }
            // Do not destroy items we didn't create, unless the user specified something else:
            if (!element.hasOwnProperty("destroyOnPop"))
                element.destroyOnPop = false
        }

        element.item.Stack.__index = element.index
        element.item.Stack.__view = root
        // Let item fill all available space by default:
        element.item.width = Qt.binding(function() { return root.width })
        element.item.height = Qt.binding(function() { return root.height })
        element.loaded = true
    }

    /*! \internal */
    function __resolveComponent(unknownObjectType, element)
    {
        // We need this extra resolve function since we dont really
        // know what kind of object the user pushed. So we try to
        // figure it out by inspecting the object:
        if (unknownObjectType.hasOwnProperty("createObject")) {
            return unknownObjectType
        } else if (typeof unknownObjectType == "string") {
            return Qt.createComponent(unknownObjectType)
        } else if (unknownObjectType.hasOwnProperty("x")) {
            return unknownObjectType
        } else if (unknownObjectType.hasOwnProperty("item")) {
            // INVARIANT: user pushed a JS-object
            element.properties = unknownObjectType.properties
            if (!unknownObjectType.item)
                unknownObjectType.item = invalidItemReplacement
            if (unknownObjectType.hasOwnProperty("destroyOnPop"))
                element.destroyOnPop = unknownObjectType.destroyOnPop
            return __resolveComponent(unknownObjectType.item, element)
        } else {
            // We cannot determine the type, so assume its a URL:
            return Qt.createComponent(unknownObjectType)
        }
    }

    /*! \internal */
    function __cleanup(element) {
        // INVARIANT: element has been removed from JSArray. Destroy its
        // item, or re-parent it back to the parent it had before it was pushed:
        var item = element.item
        if (element.destroyOnPop) {
            item.destroy()
        } else {
            // Mark the item as no longer part of the StackView. It
            // might reenter on pop if pushed several times:
            item.visible = false
            __setStatus(item, Stack.Inactive)
            item.Stack.__view = null
            item.Stack.__index = -1
            if (element.originalParent)
                item.parent = element.originalParent
        }
    }

    /*! \internal */
    function __setStatus(item, status) {
        item.Stack.__status = status
    }

    /*! \internal */
    function __performTransition(transition)
    {
        // Animate item in "outElement" out, and item in "inElement" in. Set a guard to protect
        // the user from pushing new items on signals that will fire while preparing for the transition
        // (e.g Stack.onCompleted, Stack.onStatusChanged, Stack.onIndexChanged etc). Otherwise, we will enter
        // this function several times, which causes the items to be updated half-way.
        if (__currentTransition)
            __currentTransition.animation.complete()
        __loadElement(transition.inElement)

        transition.name = transition.replace ? "replaceTransition" : (transition.push ? "pushTransition" : "popTransition")
        var enterItem = transition.inElement.item
        transition.enterItem = enterItem

        // Since an item can be pushed several times, we need to update its properties:
        enterItem.parent = root
        enterItem.Stack.__view = root
        enterItem.Stack.__index = transition.inElement.index
        __currentItem = enterItem

        if (!transition.outElement) {
            // A transition consists of two items, but we got just one. So just show the item:
            enterItem.visible = true
            __setStatus(enterItem, Stack.Activating)
            __setStatus(enterItem, Stack.Active)
            return
        }

        var exitItem = transition.outElement.item
        transition.exitItem = exitItem
        if (enterItem === exitItem)
             return

        if (root.delegate) {
            transition.properties = {
                "name":transition.name,
                "enterItem":transition.enterItem,
                "exitItem":transition.exitItem,
                "immediate":transition.immediate }
            var anim = root.delegate.getTransition(transition.properties)
            if (anim.createObject) {
                anim = anim.createObject(null, transition.properties)
                anim.runningChanged.connect(function(){ if (anim.running === false) anim.destroy() })
            }
            transition.animation = anim
        }

        if (!transition.animation) {
            console.warn("Warning: StackView: no", transition.name, "found!")
            return
        }
        if (enterItem.anchors.fill || exitItem.anchors.fill)
            console.warn("Warning: StackView: cannot transition an item that is anchored!")

        __currentTransition = transition
        __setStatus(exitItem, Stack.Deactivating)
        enterItem.visible = true
        __setStatus(enterItem, Stack.Activating)
        transition.animation.runningChanged.connect(animationFinished)
        transition.animation.start()
        // NB! For empty animations, "animationFinished" is already
        // executed at this point, leaving __animation === null:
        if (transition.immediate === true && transition.animation)
            transition.animation.complete()
    }

    /*! \internal */
    function animationFinished()
    {
        if (!__currentTransition || __currentTransition.animation.running)
            return

        __currentTransition.animation.runningChanged.disconnect(animationFinished)
        __currentTransition.exitItem.visible = false
        __setStatus(__currentTransition.exitItem, Stack.Inactive);
        __setStatus(__currentTransition.enterItem, Stack.Active);
        __currentTransition.properties.animation = __currentTransition.animation
        root.delegate.transitionFinished(__currentTransition.properties)

        if (!__currentTransition.push || __currentTransition.replace)
            __cleanup(__currentTransition.outElement)

        __currentTransition = null
    }

    /*! \internal */
    property Component invalidItemReplacement: Component {
        Text {
            width: parent.width
            height: parent.height
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        }
    }
}
