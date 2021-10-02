mouse_position_string = "0,0"
key_down_string = "";

canvas.width(300)
canvas.height(300)
canvas.use_vsync(true)
canvas.max_framerate(60)

canvas.load_font("Arial", "w:\\dev\\canvas\\arial.ttf")
context.font("20px Arial")

function canvas.update(delta)
    
end

function canvas.render()
    context.clear_rect()

    if canvas.is_key_pressed("KeyR") then
        context.fill_style("red")
    elseif canvas.is_key_pressed("KeyG") then
        context.fill_style("green")
    elseif canvas.is_key_pressed("KeyB") then
        context.fill_style("blue")
    else
        context.fill_style("black")
    end

    context.fill_text(mouse_position_string, 10, 10)

    context.fill_style("black")

    context.fill_text(key_down_string, 10, 40)

end

function canvas.onmousemove(event) 
    mouse_position_string = tostring(event.clientX) .. "," .. tostring(event.clientY)
end

function canvas.onkeydown(event)
    key_down_string = event.key or event.code
end

function canvas.onkeyup(event)
    if event.key == key_down_string or event.code == key_down_string then
        key_down_string = ""
    end
end
