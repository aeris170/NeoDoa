using System;

namespace EngineInterop {

    class Game {

        static void Main(string[] args) {
            Core c = new Core(800, 600, "NeoDoa Application Window", false);
            c.Start();
        }
    }
}