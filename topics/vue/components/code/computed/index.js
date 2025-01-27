const RootComponent = {
   data() {
      return {
         author: {
            name: 'John Doe',
            books: [
               'Vue 2 - Advanced Guide',
               'Vue 3 - Basic Guide',
               'Vue 4 - The Mystery'
            ]
         }
      }
   },
   computed: {
      // 计算属性的 getter
      publishedBooksMessage: function() {
         // `this` 指向 vm 实例
         return this.author.books.length > 0 ? 'Yes' : 'No'
      }
   },
   methods: {
      calculateBooksMessage: function() {
         return this.author.books.length > 0 ? 'Yes' : 'No'
      }
   }
}

const app = Vue.createApp(RootComponent)
const vm = app.mount('#computed-basics')

console.log(vm.calculateBooksMessage)
console.log(vm.publishedBooksMessage)
