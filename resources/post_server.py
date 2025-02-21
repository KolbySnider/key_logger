from flask import Flask, request
import logging
import threading
import tkinter as tk
from tkinter import ttk, scrolledtext
from PIL import Image, ImageTk
import io

class LogGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Flask Log Server GUI")

        # Create server controls
        self.server_frame = ttk.LabelFrame(root, text="Server Controls")
        self.server_frame.pack(padx=10, pady=5, fill=tk.X)

        self.start_btn = ttk.Button(self.server_frame, text="Start Server", command=self.start_server)
        self.start_btn.pack(side=tk.LEFT, padx=5)

        self.stop_btn = ttk.Button(self.server_frame, text="Stop Server", state=tk.DISABLED, command=self.stop_server)
        self.stop_btn.pack(side=tk.LEFT, padx=5)

        # Create log display
        self.log_frame = ttk.LabelFrame(root, text="Received Logs")
        self.log_frame.pack(padx=10, pady=5, fill=tk.BOTH, expand=True)

        self.log_text = scrolledtext.ScrolledText(self.log_frame, height=10)
        self.log_text.pack(fill=tk.BOTH, expand=True)

        # Create image display area
        self.image_frame = ttk.LabelFrame(root, text="Received Image")
        self.image_frame.pack(padx=10, pady=5, fill=tk.BOTH, expand=True)

        self.image_label = ttk.Label(self.image_frame)
        self.image_label.pack(fill=tk.BOTH, expand=True)

        # Server variables
        self.flask_thread = None
        self.server_running = False

    def start_server(self):
        if not self.server_running:
            self.flask_thread = threading.Thread(target=self.run_flask_server, daemon=True)
            self.flask_thread.start()
            self.server_running = True
            self.start_btn.config(state=tk.DISABLED)
            self.stop_btn.config(state=tk.NORMAL)
            self.log("Server started on http://0.0.0.0:5000")

    def stop_server(self):
        if self.server_running:
            self.server_running = False
            self.start_btn.config(state=tk.NORMAL)
            self.stop_btn.config(state=tk.DISABLED)
            self.log("Server stop requested - use CTRL+C in terminal to force stop")

    def log(self, message):
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)

    def display_image(self, pil_image):
        # Convert PIL image to Tkinter PhotoImage
        self.tk_image = ImageTk.PhotoImage(pil_image)
        self.image_label.config(image=self.tk_image)
        self.log("Image displayed.")

    def run_flask_server(self):
        app = Flask(__name__)
        logging.basicConfig(level=logging.INFO)

        @app.route('/log', methods=['POST'])
        def log_post():
            # Process text log messages
            data = request.get_json(silent=True) or request.data.decode('utf-8')
            log_message = f"Received POST data: {data}"
            self.root.after(0, self.log, log_message)
            return "POST received and logged.", 200

        @app.route('/image', methods=['POST'])
        def image_post():
            # Expect raw binary image data
            image_data = request.data
            try:
                # Open image from bytes using Pillow
                pil_image = Image.open(io.BytesIO(image_data))
                # Optional: Convert image mode if needed
                if pil_image.mode != "RGB":
                    pil_image = pil_image.convert("RGB")
                # Update GUI safely from Flask thread
                self.root.after(0, self.display_image, pil_image)
                return "Image received and displayed.", 200
            except Exception as e:
                self.root.after(0, self.log, f"Failed to process image: {e}")
                return "Failed to process image.", 400

        # Run Flask server without debug mode
        app.run(host='0.0.0.0', port=5000, debug=False)

if __name__ == "__main__":
    root = tk.Tk()
    gui = LogGUI(root)
    root.mainloop()
