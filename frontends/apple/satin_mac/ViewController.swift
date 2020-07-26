

import Cocoa
import MetalKit

class ViewController: NSViewController {

    var renderer:  ViewDelegate!
    var mtkView: MTKView!
    var panRecognizer: NSPanGestureRecognizer!
    var clickRecognizer: NSClickGestureRecognizer!
    var doubleClickRecognizer: NSClickGestureRecognizer!

    override func viewDidLoad() {
        super.viewDidLoad()
        guard let mtkView = view as? MTKView else {
            print("The view is not a MTKView!")
            return
        }

        panRecognizer = NSPanGestureRecognizer(target: self, action: #selector(pan(recognizer:)))
        mtkView.addGestureRecognizer(panRecognizer)
        clickRecognizer = NSClickGestureRecognizer(target: self, action: #selector(click(recognizer:)))
        mtkView.addGestureRecognizer(clickRecognizer)
        doubleClickRecognizer = NSClickGestureRecognizer(target: self, action: #selector(double_click(recognizer:)))
        mtkView.addGestureRecognizer(doubleClickRecognizer)
        doubleClickRecognizer.numberOfClicksRequired = 2

        NSEvent.addLocalMonitorForEvents(matching: .keyDown) {
            self.keyDown(with: $0)
            //return $0
            return nil
        }
        
        NSEvent.addLocalMonitorForEvents(matching: .keyUp) {
            self.keyUp(with: $0)
            return $0
        }
        
        if let device = MTLCreateSystemDefaultDevice(){
            mtkView.device = device
            
            renderer = ViewDelegate(view: mtkView)
            mtkView.delegate = renderer
        }else{
            print("Error creating system default device")
        }
    }

    override var representedObject: Any? {
        didSet {
        // Update the view, if already loaded.
        }
    }
    
    override func keyDown(with event: NSEvent) {
    //TODO(Vidar):This is really hacky and will not work properly
    //... I dont know Swift!
        for u in event.characters!.uppercased().unicodeScalars{
            renderer.input_state.keys_typed.0 = u.value as UInt32;
            renderer.input_state.num_keys_typed = 1;
            keys_down![NSNumber(value:u.value)] = NSNumber(1);
        }
    }
    
    override func keyUp(with event: NSEvent) {
        for u in event.characters!.uppercased().unicodeScalars{
            keys_down![NSNumber(value:u.value)] = NSNumber(0);
        }
    }
    
    override func mouseDown(with event: NSEvent) {
        renderer.input_state.mouse_down = 1;
    }
    
    override func mouseUp(with event: NSEvent) {
        renderer.input_state.mouse_down = 0;
    }
    
    @objc func pan(recognizer: NSPanGestureRecognizer) {
        let p = recognizer.location(in: recognizer.view);
        let s = recognizer.view!.frame.size;
        var inv_size = 1.0/s.height;
        var offset:[CGFloat] = [0.0, 0.0];
        if( s.width < s.height){
            inv_size = 1.0/s.width;
            offset[1] = (s.height - s.width)*0.5;
        }else{
            offset[0] = (s.width - s.height)*0.5;
        }
        renderer.input_state.mouse_x = Float((p.x - offset[0]) * inv_size);
        renderer.input_state.mouse_y = 1.0-Float((p.y - offset[1]) * inv_size);
        if(recognizer.state == NSGestureRecognizer.State.ended){
            renderer.input_state.mouse_state = Int32(MOUSE_DRAG_RELEASE.rawValue);
        }else{
            renderer.input_state.mouse_state = Int32(MOUSE_DRAG.rawValue);
        }
    }
    
    @objc func click(recognizer: NSClickGestureRecognizer) {
        let p = recognizer.location(in: recognizer.view);
        let s = recognizer.view!.frame.size;
        var inv_size = 1.0/s.height;
        var offset:[CGFloat] = [0.0, 0.0];
        if( s.width < s.height){
            inv_size = 1.0/s.width;
            offset[1] = (s.height - s.width)*0.5;
        }else{
            offset[0] = (s.width - s.height)*0.5;
        }
        renderer.input_state.mouse_x = Float((p.x - offset[0]) * inv_size);
        renderer.input_state.mouse_y = 1.0-Float((p.y - offset[1]) * inv_size);
        renderer.input_state.mouse_state = Int32(MOUSE_CLICKED.rawValue);
    }
    
    @objc func double_click(recognizer: NSClickGestureRecognizer) {
        let p = recognizer.location(in: recognizer.view);
        let s = recognizer.view!.frame.size;
        var inv_size = 1.0/s.height;
        var offset:[CGFloat] = [0.0, 0.0];
        if( s.width < s.height){
            inv_size = 1.0/s.width;
            offset[1] = (s.height - s.width)*0.5;
        }else{
            offset[0] = (s.width - s.height)*0.5;
        }
        renderer.input_state.mouse_x = Float((p.x - offset[0]) * inv_size);
        renderer.input_state.mouse_y = 1.0-Float((p.y - offset[1]) * inv_size);
        /*renderer.input_state.mouse_state = Int32(MOUSE_DOUBLECLICKED.rawValue);*/
    }
    
}

