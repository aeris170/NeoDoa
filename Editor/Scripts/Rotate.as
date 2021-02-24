enum Axis { X, Y, Z }

class Rotate : Module {
    Transform@ ownTransform;
    Axis axis = Y;
    int speed;

    void Execute(float delta) {
        @ownTransform = cast<Transform>(Get("Transform"));
        if(ownTransform !is null) {
            if(axis == X) {
                ownTransform.Rotation *= quat(vec3(toRadians(speed * delta), 0, 0));
            }
            if(axis == Y) {
                ownTransform.Rotation *= quat(vec3(0, toRadians(speed * delta), 0));
            }
            if(axis == Z) {
                ownTransform.Rotation *= quat(vec3(0, 0, toRadians(speed * delta)));
            }
        }
    }
}
