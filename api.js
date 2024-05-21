const express = require('express');
const router = express();
router.use(express.json());

var inferenceFound = false;
var inference;
router.post('/api/postResult', (req, res) => {
    inference = req.body;
    inferenceFound = true;
    //res.end(JSON.stringify(inference));
    res.sendStatus(200);
});

const noInference = {
    "noInference": "none"
}
router.get('/api/getResult', (req, res) => {
    if (inferenceFound == true) {
        inferenceFound = false;
        try {
            res.end(JSON.stringify(inference));
            return;
        } catch (err) {
            console.log(err);
        }
    }
    res.end(JSON.stringify(noInference));
});

router.listen(3000, () => {
    console.log("Server is running");
});

