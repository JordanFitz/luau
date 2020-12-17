keys = {}

offset = {}
offset.x = 0
offset.y = 0

move_amt = 100

function is_key_down(name)
    return keys[name] ~= nil and keys[name]
end

function canvas.update(delta)
    m = move_amt * delta

    if is_key_down("ArrowUp") then
        offset.y = offset.y - m
    end

    if is_key_down("ArrowDown") then
        offset.y = offset.y + m
    end

    if is_key_down("ArrowRight") then
        offset.x = offset.x + m
    end

    if is_key_down("ArrowLeft") then
        offset.x = offset.x - m
    end
end

function canvas.render()
    context.clear_rect()

    context.line_width(10)
    context.stroke_rect(offset.x + 75, offset.y + 140, 150, 110)
    context.fill_rect(offset.x + 130,  offset.y + 190, 40, 60)

    context.begin_path()
    context.move_to(offset.x + 50, offset.y + 140)
    context.line_to(offset.x + 150, offset.y + 60)
    context.line_to(offset.x + 250, offset.y + 140)
    context.close_path()
    context.stroke()
end

canvas.width(1280)
canvas.height(720)
canvas.use_vsync(true)
canvas.max_framerate(60)

function canvas.onkeydown(event)
    keys[event.code] = true
end

function canvas.onkeyup(event)
    keys[event.code] = false
end
