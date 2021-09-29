mouse_position_string = "0,0"

canvas.width(300)
canvas.height(300)
canvas.use_vsync(true)
canvas.max_framerate(60)

canvas.load_font("Arial", "w:\\dev\\canvas\\arial.ttf")
context.font("20px Arial")context.fill_style("green")

function canvas.update(delta)
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
end

function canvas.render()

end

function canvas.onmousemove(event) 
    mouse_position_string = tostring(event.clientX) .. "," .. tostring(event.clientY)
end
