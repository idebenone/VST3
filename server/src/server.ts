import express, { Express, Request, Response } from 'express';
import morgan from "morgan";
import cors from "cors";
import multer from "multer";
import path from "path";
import fs from "fs";

const app: Express = express();

app.use(morgan('dev'))
app.use(express.json());
app.use(cors());

app.listen(3000, () => {
    console.log(`⚡ | Server is running at http://localhost:${process.env.SERVER_PORT}`);
})

const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        const uploadDir = path.join(__dirname, "uploads");
        if (!fs.existsSync(uploadDir)) {
            fs.mkdirSync(uploadDir);
        }
        cb(null, uploadDir);
    },
    filename: (req, file, cb) => {
        const uniqueName = `${Date.now()}-${file.originalname}`;
        cb(null, uniqueName);
    },
});

const upload = multer({ storage });

// POST endpoint to handle audio file upload
app.post("/upload", upload.single("audio"), (req: Request, res: Response): void => {
    if (!req.file) {
        res.status(400).json({ error: "No file uploaded." });
        return
    }

    console.log(`File uploaded: ${req.file.filename}`);
    res.status(200).json({
        message: "File uploaded successfully.",
        fileName: req.file.filename,
    });
});