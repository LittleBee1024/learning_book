const RootComponent = {
   data() {
      return {
         question: '',
         answer: 'Questions usually contain a question mark. ;-)'
      }
   },
   watch: {
      // 每当 question 发生变化时，该函数将会执行
      question(newQuestion, oldQuestion) {
         if (newQuestion.indexOf('?') > -1) {
            this.getAnswer()
         }
      }
   },
   methods: {
      getAnswer() {
         this.answer = 'Thinking...'
         axios
            .get('https://yesno.wtf/api')
            .then(response => {
               this.answer = response.data.answer
            })
            .catch(error => {
               this.answer = 'Error! Could not reach the API. ' + error
            })
      }
   }
}

const app = Vue.createApp(RootComponent)
const vm = app.mount('#watch-example')

